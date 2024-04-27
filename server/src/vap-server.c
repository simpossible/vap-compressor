//
//  vap-server.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/18.
//

#include "vap-server.h"
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

static struct Dictionary * compress_dic = NULL;




static int handleHellowWord(struct mg_connection *conn, void *ignored)
{
    printf("hahaha");
    const char *msg = "Hello world";
    unsigned long len = (unsigned long)strlen(msg);
    char *test = getParamsFromRequest(conn, "test");
    if (test != NULL){
        printf("a");
    }
    mg_send_http_ok(conn, "text/plain", len);
    
    mg_write(conn, msg, len);
    
    return 200; /* HTTP state 200 = OK */
}



static int onFileRequest(struct mg_connection *conn, void *ignored) {
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
                if (isFileInnerPath(entry->d_name)) {
                    continue;
                }
                char *absulutePath = osJoinPath(filePath, entry->d_name);
                struct stat dirStat;
                if(stat(absulutePath, &dirStat) < 0) {
                    free(absulutePath);
                    continue;
                }
                if (string_end_with(entry->d_name, ".mp4")) {
                    cJSON* vapInfo = getVapInfo(absulutePath);
                    if (vapInfo != NULL) {
                        absuluteSubFiles = char_add_element(absuluteSubFiles, &arrayLength, absulutePath);
                    }
                }
                if (S_ISDIR(dirStat.st_mode)) {
                    absuluteSubFiles = char_add_element(absuluteSubFiles, &arrayLength, absulutePath);
                }
                free(absulutePath);
            }
            
        }
        cJSON *subFiles = cJSON_CreateStringArray(absuluteSubFiles, arrayLength);
        free(absuluteSubFiles);
        cJSON_AddItemToObject(file_info, "sub_files", subFiles);
        
        closedir(dir);
        
    }
//    result["code"] = 0
//        result["msg"] = ""
//        result["file_info"] = file_info
//        result["is_dir"] = isDir
//        result["is_vap"] = isVap
//        console.log("result:", result)
//        res.writeHead(200, { 'Content-Type': 'application/jsonn' })
//        res.end(JSON.stringify(result))
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
    
    return 200;
}



void startVapServer(void) {
    
    // 初始化压缩的字典
    compress_dic = dicCreate();
    
    /* Initialize CivetWeb library without OpenSSL/TLS support. */
    mg_init_library(0);
    /* Start the server using the advanced API. */
    struct mg_callbacks callbacks = {0};
    void *user_data = NULL;
    
    struct mg_init_data mg_start_init_data = {0};
    mg_start_init_data.callbacks = &callbacks;
    mg_start_init_data.user_data = user_data;
    const char *documentPath = vapServerWorkSpacePath();
    const char *options[] = {
        "document_root", documentPath,
        "listening_ports", "8070",
        NULL
    };
    mg_start_init_data.configuration_options = options;
    
    struct mg_error_data mg_start_error_data = {0};
    char errtxtbuf[256] = {0};
    mg_start_error_data.text = errtxtbuf;
    mg_start_error_data.text_buffer_size = sizeof(errtxtbuf);
    
    struct mg_context *ctx =
    mg_start2(&mg_start_init_data, &mg_start_error_data);
    if (!ctx) {
        fprintf(stderr, "Cannot start server: %s\n", errtxtbuf);
        return;
    }
    mg_set_request_handler(ctx, "/hello", handleHellowWord, "Hello world");
    mg_set_request_handler(ctx, "/file", onFileRequest, "file");
    
}
