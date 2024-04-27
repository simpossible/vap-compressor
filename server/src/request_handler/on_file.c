//
//  on_file.c
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#include <stdio.h>
#include "civetweb.h"
#include <string.h>
#include "server_util.h"
#include "dictionary.h"
#include <stdint.h>
#include "CJSON.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include "vap_parser.h"
#include "transcode.h"
#include <dirent.h>
#include <stdlib.h>

int onFileRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    if (file_exists(filePath) == -1) {
        cJSON *result = cJSON_CreateObject();
        cJSON_AddStringToObject(result, "msg", "file not exist");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        cJSON_AddItemToObject(result, "file_info", cJSON_CreateObject());
        char *resultStr = cJSON_Print(result);
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        cJSON_Delete(result);
        free(filePath);
        return 200;
    }
    struct stat fileStat;
    if(stat(filePath, &fileStat) < 0) {
        return 404;
    }
    bool isDir = S_ISDIR(fileStat.st_mode);
    bool isVap = false;
    cJSON *file_info = cJSON_CreateObject();
    if (!isDir) {
        cJSON_AddNumberToObject(file_info, "size", fileStat.st_size);
        cJSON_AddItemToObject(file_info, "sub_files", cJSON_CreateObject());
        if (string_end_with(filePath, ".mp4")) {
            cJSON* vapInfo = getVapInfo(filePath);
            if (vapInfo != NULL) {
                VideoInfo * videoInfo = getMp4Info(filePath);
                if (videoInfo != NULL) {
                   cJSON *videoInfoJson = videoInfnToJson(videoInfo);
                    cJSON_AddItemToObject(file_info, "video_info", videoInfoJson);
                }
                isVap = true;
                cJSON_AddItemToObject(file_info, "vap_info", vapInfo);
            }
        }
    }else {
        //var subFiles = fs.readdirSync(filePath)
        char **absuluteSubFiles = NULL;
        int arrayLength = 0;
        struct dirent *entry;
        DIR *dir = opendir(filePath);
        if (dir != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                //join path
                int len = strlen(entry->d_name);
                char *subName = malloc(len + 1);
                strcpy(subName, entry->d_name);
                if (isFileInnerPath(subName)) {
                    free(subName);
                    continue;
                }
                char *absulutePath = osJoinPath(filePath, subName);
                struct stat dirStat;
                if(stat(absulutePath, &dirStat) < 0) {
                    free(absulutePath);
                    free(subName);
                    continue;
                }
                if (string_end_with(subName, ".mp4")) {
                    cJSON* vapInfo = getVapInfo(absulutePath);
                    if (vapInfo != NULL) {
                        absuluteSubFiles = char_add_element(absuluteSubFiles, &arrayLength, absulutePath);
                        cJSON_Delete(vapInfo);
                    }
                }
                if (S_ISDIR(dirStat.st_mode)) {
                    absuluteSubFiles = char_add_element(absuluteSubFiles, &arrayLength, absulutePath);
                }
                free(subName);
                free(absulutePath);
            }
            
        }
        cJSON *subFiles = cJSON_CreateStringArray(absuluteSubFiles, arrayLength);
        free(absuluteSubFiles);
        cJSON_AddItemToObject(file_info, "sub_files", subFiles);
        closedir(dir);
        
    }
    cJSON *result = cJSON_CreateObject();
    cJSON_AddNumberToObject(result, "code", 0);
    cJSON_AddStringToObject(result, "msg", "");
    cJSON_AddItemToObject(result, "file_info", file_info);
    cJSON_AddBoolToObject(result, "is_dir", isDir);
    cJSON_AddBoolToObject(result, "is_vap", isVap);
    char *resultStr = cJSON_Print(result);
    unsigned long len = strlen(resultStr);
    mg_send_http_ok(conn, "application/jsonn", len);
    mg_write(conn, resultStr, len);
    free(filePath);
    cJSON_Delete(result);
    
    return 200;
}
