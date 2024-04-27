//
//  on_vap_file_list.c
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#include "civetweb.h"
#include "CJSON.h"
#include <stdlib.h>
#include "vap_parser.h"
#include "transcode.h"
#include "server_util.h"
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "server_util.h"

void get_vap_list_recursive(cJSON *list, char * filePath);

int onVapFileListRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    cJSON *result = cJSON_CreateObject();
    cJSON *file_info = cJSON_CreateObject();
    cJSON *vapFiles = cJSON_CreateArray();
    if (file_exists(filePath) == -1) {
        cJSON_AddStringToObject(result, "msg", "file not exist");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        cJSON_AddItemToObject(result, "file_info", file_info);
        goto finish;
    }
    struct stat fileStat;
    if(stat(filePath, &fileStat) < 0) {
        goto finish;
    }
    bool isDir = S_ISDIR(fileStat.st_mode);
    if (!isDir) {
        cJSON_AddStringToObject(result, "msg", "file not directory");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        cJSON_AddItemToObject(result, "file_info", file_info);
        goto finish;
    }
    
    cJSON_AddNumberToObject(result, "code", 0);
    cJSON_AddStringToObject(result, "msg", "");
    get_vap_list_recursive(vapFiles, filePath);
    cJSON_AddItemToObject(result, "file_list", vapFiles);
    
finish:
    {
        char *resultStr = cJSON_Print(result);
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        cJSON_Delete(result);
        cJSON_Delete(file_info);
        free(filePath);
    }
    return 200;
    
}


void get_vap_list_recursive(cJSON *list, char * filePath) {
    VapFileInfo *info =  getFileInfoOfVap(filePath);
    if (info != NULL) {
        cJSON *infoJson = cJSON_CreateObject();
        cJSON_AddStringToObject(infoJson, "path", info->path);
        cJSON_AddNumberToObject(infoJson, "size", info->size);
        if (info->video_info != NULL) {
            cJSON *videoInfoJson = videoInfnToJson(info->video_info);
            cJSON_AddItemToObject(infoJson, "video_info", videoInfoJson);
        }
        if (info->vap_info != NULL) {
            cJSON_AddItemToObject(infoJson, "vap_info", info->vap_info);
        }
        cJSON_AddItemToArray(list, infoJson);
        free(info->video_info);
        free(info);
        return;
    }
    
    DIR *dir = opendir(filePath);
    struct dirent *entry;
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            //join path
            long len = strlen(entry->d_name);
            char *subName = malloc(len + 1);
            strcpy(subName, entry->d_name);
            if ((strcmp(subName, ".") == 0) || strcmp(subName, "..") == 0) {
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
            get_vap_list_recursive(list, absulutePath);
            free(subName);
            free(absulutePath);
        }
        
    }
}
