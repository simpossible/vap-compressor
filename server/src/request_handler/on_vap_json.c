//
//  on_vap_json.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
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


int onVapJsonRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    if (file_exists(filePath) == -1) {
        char *resultStr = "not exist";
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        free(filePath);
        return 404;
    }
    cJSON *vapJson = getVapcInfo(filePath);
    if (vapJson == NULL) {
        char *resultStr = "not vap file";
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        free(filePath);
        return 404;
    }
    char *resultStr = cJSON_Print(vapJson);
    unsigned long len = strlen(resultStr);
    mg_send_http_ok(conn, "application/jsonn", len);
    mg_write(conn, resultStr, len);
    cJSON_Delete(vapJson);
    free(filePath);
    return 200;
}
