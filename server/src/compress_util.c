//
//  compress_util.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
//

#include "compress_util.h"
#include <string.h>
#include <pthread.h>


static Dictionary *shareDictionary = NULL;
static pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER;

void initialDic(void) {
    pthread_mutex_lock(&sharedLock);
    if (shareDictionary == NULL) {
        shareDictionary = dicCreate();
    }
    pthread_mutex_unlock(&sharedLock);
}


CompressInfo * cacheGetCompressInfo(char *filePath) {
    initialDic();
    CompressInfo *exist = (CompressInfo *)dicGetValue(shareDictionary, filePath);
    return exist;
    
}

void cacheSaveCompressInfo(char *filePath, CompressInfo *info) {
    initialDic();
    dicSetValue(shareDictionary, filePath, info);
}

extern cJSON *compressInfnToJson(CompressInfo *compressInfo) {
    cJSON *json =  cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "state", compressInfo->state);
    if (compressInfo->org_path != NULL) {
        cJSON_AddStringToObject(json, "org_path", compressInfo->org_path);
    }
    if (compressInfo->outputPath != NULL) {
        cJSON_AddStringToObject(json, "org_path", compressInfo->outputPath);
    }
    if (compressInfo->outputFileInfo != NULL) {
        cJSON_AddItemToObject(json, "outputFileInfo", compressInfo->outputFileInfo);
    }
    return json;
}



