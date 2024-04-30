//
//  on_clear_files.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/29.
//

#include <stdio.h>
#include "vap_parser.h"
#include "transcode.h"
#include "server_util.h"
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "server_util.h"
#include <stdlib.h>
#include "compress_util.h"

int onClearFilesRequest(struct mg_connection *conn, void *ignored) {
    cJSON *result =NULL;
    cJSON *params = getPostFromRequest(conn);
    if (params != NULL) {
       cJSON *filesArray = cJSON_GetObjectItem(params, "files");
        int len = cJSON_GetArraySize(filesArray);
        for (int i =0; i < len; i ++) {
            cJSON *fileJson = cJSON_GetArrayItem(filesArray, i);
            char *filePath =  cJSON_GetStringValue(fileJson);
            char *tempVapPath = tempVapPathFrom(filePath);
            if (file_exists(tempVapPath) != -1 ) {
                rename(tempVapPath, filePath);
            }
            cacheDeleteCompressInfo(filePath);
        }
        cJSON_Delete(params);
    }
finish:
    {
        char *resultStr = "";
        char *printstr = NULL;
        if (result != NULL) {
            printstr = cJSON_Print(result);
            if (printstr != NULL) {
                resultStr = printstr;
            }
        }
        unsigned long len = strlen(resultStr);
            mg_send_http_ok(conn, "application/jsonn", len);
            mg_write(conn, resultStr, len);
        if (result != NULL) {
            cJSON_Delete(result);
        }
        if (printstr != NULL) {
            free(printstr);
        }
    }
    return 200;
}
