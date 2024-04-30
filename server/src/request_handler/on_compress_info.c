//
//  on_compress_info.c
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
#include "compress_util.h"


int onCompressInfoRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    cJSON *result = NULL;
    if (file_exists(filePath) == -1) {
        cJSON_AddStringToObject(result, "msg", "file not exist");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        goto finish;
    }
    
    CompressInfo *compressInfo = cacheGetCompressInfo(filePath);
    if (compressInfo != NULL) {
        result = compressInfnToJson(compressInfo);
        goto finish;
    }
    char * tempPath = tempVapPathFrom(filePath);
    if (file_exists(tempPath) != -1) {
        cJSON *vapInfo =  getVapcInfo(tempPath);
        if (vapInfo == NULL) {
            // find temp path but not a vap file
            CompressInfo tempCompressInfo = {
                .org_path = filePath,
                .state = CompressState_none,
            };
            result =compressInfnToJson(&tempCompressInfo);
            goto finish;
        }else {
            cJSON *vapFileInfoJson = getVapFileInfoJson(tempPath);
            // 这里用堆来创建对象是应为要存下来
            compressInfo = malloc(sizeof(CompressInfo));
            compressInfo->org_path = tempPath;
            compressInfo->state = CompressState_done;
            compressInfo->outputPath = tempPath;
            compressInfo->outputFileInfo = vapFileInfoJson;
            result =compressInfnToJson(compressInfo);
            cacheSaveCompressInfo(filePath, compressInfo);
            goto finish;
        }
    }else {
        CompressInfo tempCompressInfo = {
            .state = CompressState_none,
        };
        result =compressInfnToJson(&tempCompressInfo);
        goto finish;
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
        free(filePath);
        if (printstr != NULL) {
            free(printstr);
        }
    }
    return 200;
    
}
