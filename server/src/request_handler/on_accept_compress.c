//
//  on_accept_compress.c
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

int onAcceptCompressnRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    cJSON *result = NULL;
    if (file_exists(filePath) == -1) {
        char *resultStr = "not exist";
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        free(filePath);
        return 404;
    }
    if (file_exists(filePath)) {
        remove(filePath);
    }    
    
    char *tempVapPath = tempVapPathFrom(filePath);
    if (file_exists(tempVapPath) != -1) {
        rename(tempVapPath, filePath);
    }
    cacheDeleteCompressInfo(filePath);

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
        free(filePath);
        if (printstr != NULL) {
            free(printstr);
        }
    }
    return 200;
}
