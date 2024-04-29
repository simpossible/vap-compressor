//
//  onDrag.c
//  Vap
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

extern char * getAppDragFiles(void);
// 网页端不能拿到路径。就让它从这里来拿
int onDragRequest(struct mg_connection *conn, void *ignored) {
    
    char * fielsJsonStr = getAppDragFiles();
    cJSON *result = cJSON_CreateRaw(fielsJsonStr);
    
    {
        char *resultStr = "";
        if (result != NULL) {
            resultStr = cJSON_Print(result);
            cJSON_Delete(result);
        }
        
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
    }
    free(fielsJsonStr);
    return 200;
}
