//
//  on_compress.c
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
#include <pthread.h>
void _compressFile(char *filePath);

int onStartCompressRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    cJSON *result = NULL;
    if (file_exists(filePath) == -1) {
        cJSON_AddStringToObject(result, "msg", "file not exist");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        goto finish;
    }
    
    _compressFile(filePath);
    
finish:
    {
        char *resultStr = NULL;
        if (result != NULL) {
            cJSON_Print(result);
        }else {
            resultStr = "";
        }
        
        unsigned long len = strlen(resultStr);
        mg_send_http_ok(conn, "application/jsonn", len);
        mg_write(conn, resultStr, len);
        cJSON_Delete(result);
        free(filePath);
    }
    return 200;
    
}

void *__compressVapFile(void *args) {
    char * filePath = (char *)args;
    char *outputPath = tempVapPathFrom(filePath);
    int ret = av_compress_video(filePath, outputPath);
    if (ret != 0) {
        printf("compress fail");
        return NULL;
    }
    
    CompressInfo *compressInfo = cacheGetCompressInfo(filePath);    
    if (compressInfo == NULL) {
        printf("no compress info");
        remove(outputPath);
        return NULL;
    }
    compressInfo->state = CompressState_done;
    VapFileInfo *outputVapFileInfo = getVapFileInfoAllowNotVap(outputPath);
    if (outputVapFileInfo == NULL) {
        compressInfo->errorMsg = "out put file error";
        compressInfo->errorCode = -2;
        return NULL;
    }
        
    char *oldVapcContent = getVapcContent(filePath);
    if (oldVapcContent == NULL) {
        compressInfo->errorMsg = "old vap error";
        compressInfo->errorCode = -2;
        return NULL;
    }
    // add vap info to outoup    
    ret = addVapcToMp4File(outputPath, oldVapcContent);
    if (ret != 0) {
        printf("add vapc error");
        remove(outputPath);
        compressInfo->errorMsg = "add vapc error";
        compressInfo->errorCode = ret;
        return NULL;
    }
    
    if (compressInfo->auto_accept) {
        struct stat inputFileStat;
        struct stat ouputFileStat;
        stat(filePath, &inputFileStat);
        stat(outputPath, &ouputFileStat);
        if (inputFileStat.st_size < ouputFileStat.st_size) {
            //delete output
            remove(outputPath);
            compressInfo->progress = 100;
            compressInfo->errorCode = -2;
            compressInfo->errorMsg = "压缩后变大了";
        }else {
            remove(filePath);
            rename(outputPath, filePath);
            compressInfo->progress = 100;
        }
        
    }else {
        compressInfo->progress = 100;
    }
    return NULL;
}

void _compressFile(char *filePath) {
    pthread_t thread_id;
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        printf("Before Thread\n");
        pthread_create(&thread_id, &attr, __compressVapFile, filePath);
}


