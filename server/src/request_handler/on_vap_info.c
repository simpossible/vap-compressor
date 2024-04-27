//
//  on_vap_info.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
//

#include <stdio.h>
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


int onVapInfoRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    cJSON *result = cJSON_CreateObject();
    cJSON *file_info = cJSON_CreateObject();
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
    if (isDir) {
        cJSON_AddStringToObject(result, "msg", "is_dir");
        cJSON_AddTrueToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        cJSON_AddItemToObject(result, "file_info", file_info);
        goto finish;
    }
    cJSON *jsonInfo =  getVapInfo(filePath);
    if (jsonInfo == NULL) {
        cJSON_AddStringToObject(result, "msg", "not vap");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        cJSON_AddItemToObject(result, "file_info", file_info);
        goto finish;
    }
        
    
    cJSON_AddNumberToObject(result, "code", 0);
    cJSON_AddStringToObject(result, "msg", "");
    cJSON_AddItemToObject(result, "vap_info", jsonInfo);
    cJSON_AddFalseToObject(result, "is_dir");
    cJSON_AddTrueToObject(result, "is_vap");
    
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
