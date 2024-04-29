//
//  on_quit_compress.c
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

int onQuitCompressnRequest(struct mg_connection *conn, void *ignored) {
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
    cacheDeleteCompressInfo(filePath);
    
    char *tempVapPath = tempVapPathFrom(filePath);
    if (file_exists(tempVapPath)) {
        remove(tempVapPath);
    }

finish:
    {
        char *resultStr = "";
        if (result != NULL) {
            resultStr = cJSON_Print(result);
            cJSON_Delete(result);
        }
        
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        
        free(filePath);
    }
    return 200;
}
