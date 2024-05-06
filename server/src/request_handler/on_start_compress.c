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
#include "vap_parser.h"
void _compressFile(char *filePath, char *crf, char *preset);

extern int onCompressInfoRequest(struct mg_connection *conn, void *ignored) ;

int onStartCompressRequest(struct mg_connection *conn, void *ignored) {
    cJSON *result = NULL;
    char *filePath = getParamsFromRequest(conn, "path");
    if (file_exists(filePath) == -1) {
        cJSON_AddStringToObject(result, "msg", "file not exist");
        cJSON_AddFalseToObject(result, "is_dir");
        cJSON_AddFalseToObject(result, "is_vap");
        cJSON_AddNumberToObject(result, "code", -1);
        goto finish;
    }
    
    char *crf = getParamsFromRequest(conn, "quality");
    char *preset = getParamsFromRequest(conn, "speed");
    char * auto_accept = getParamsFromRequest(conn, "auto_accept");
    bool need_auto_accpet = false;
    if (auto_accept != NULL) {
        if (strcmp(auto_accept, "true") == 0) {
            need_auto_accpet = true;
        }
    }
    
    char *tempVapPath = tempVapPathFrom(filePath);
    if (file_exists(tempVapPath) != -1) {
        BoxArray box = getVapBoxes(tempVapPath);
        if (box.state == 1) {
            remove(tempVapPath);
        }else {
            free(tempVapPath);
            return onCompressInfoRequest(conn, ignored);
        }
        
    }
    
    CompressInfo *exist = cacheGetCompressInfo(filePath);
    if (exist != NULL && exist->state == CompressState_compressing) {
        cJSON *result = compressInfnToJson(exist);
        goto finish;
    }
    
    if (exist == NULL) {
        exist = malloc(sizeof(CompressInfo));
        memset(exist, 0, sizeof(CompressInfo));
    }
    exist->state = CompressState_compressing;
    exist->auto_accept = need_auto_accpet;
    exist->org_path = filePath;
    cacheSaveCompressInfo(filePath, exist);
    
    
    if (crf == NULL || strcmp(crf, "") == 0) {
        crf = "23";
    }
    if (preset == NULL || strcmp(preset, "") == 0) {
        preset = "slow";
    }
    
    _compressFile(filePath, crf, preset);
    
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

void __onCommpressProgressChange(char *fileName, float progress) {
    CompressInfo *compressInfo = cacheGetCompressInfo(fileName);
    compressInfo->progress = progress;
    if (compressInfo == NULL) {
        printf("no compress info");
    }
    printf("==========fileName is%s progress is %f \n", fileName, progress);
}

void *__compressVapFile(void **args) {
    char **params = (char **)args;
    char * filePath = params[0];
    char * crf = params[1];
    char * preset = params[2];
    char *outputPath = tempVapPathFrom(filePath);
    int ret = av_compress_video(filePath, outputPath, crf, preset, __onCommpressProgressChange);
    if (ret != 0) {
        printf("compress fail");
        goto end;;
    }
    
    CompressInfo *compressInfo = cacheGetCompressInfo(filePath);
    if (compressInfo == NULL) {
        printf("no compress info");
        remove(outputPath);
        goto end;;
    }
    
    VapFileInfo *outputVapFileInfo = getVapFileInfoAllowNotVap(outputPath);
    if (outputVapFileInfo == NULL) {
        compressInfo->state = CompressState_done;
        compressInfo->errorMsg = "out put file error";
        compressInfo->errorCode = -2;
        compressInfo->progress = 100;
        goto end;
    }
        
    char *oldVapcContent = getVapcContent(filePath);
    if (oldVapcContent == NULL) {
        compressInfo->state = CompressState_done;
        compressInfo->errorMsg = "old vap error";
        compressInfo->errorCode = -2;
        goto end;;
    }
    // add vap info to outoup    
    ret = addVapcToMp4File(outputPath, oldVapcContent);
    if (ret != 0) {
        printf("add vapc error");
        remove(outputPath);
        compressInfo->state = CompressState_done;
        compressInfo->errorMsg = "add vapc error";
        compressInfo->errorCode = ret;
        goto end;
    }
    
    cJSON *vapFileInfoJson = getVapFileInfoJson(outputPath);
    if (vapFileInfoJson != NULL) {
        compressInfo->outputFileInfo = vapFileInfoJson;
    }
    compressInfo->state = CompressState_done;
    
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
end:
    free(args);
    return NULL;
}

char * copyChar(char *org){
    char *new = malloc(strlen(org) + 1);
    strcpy(new, org);
    return new;
}

void _compressFile(char *filePath, char *crf, char *preset) {
    pthread_t thread_id;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    printf("Before Thread\n");
    void **params = malloc(3 * sizeof(char *));
    params[0] = copyChar(filePath);
    params[1] = copyChar(crf);
    params[2] = copyChar(preset);
    pthread_create(&thread_id, &attr, __compressVapFile, params);
}



