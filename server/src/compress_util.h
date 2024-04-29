//
//  compress_util.h
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
//

#ifndef compress_util_h
#define compress_util_h

#include <stdio.h>
#include "CJSON.h"
#include "dictionary.h"
#include <stdbool.h>

#endif /* compress_util_h */

typedef enum {
    CompressState_error = -1,
    CompressState_none = 0,
    CompressState_compressing = 1,
    CompressState_done = 2,
    CompressState_acceptting = 3,
    CompressState_quitting = 4
} CompressState;


typedef struct CompressInfo {
    CompressState state;
    char * org_path;
    char * outputPath;
    cJSON * outputFileInfo;
    bool auto_accept;
    float progress;
    int errorCode;
    char * errorMsg;
} CompressInfo;


extern CompressInfo* cacheGetCompressInfo(char *filePath);

extern void cacheSaveCompressInfo(char *filePath, CompressInfo *info);
extern void cacheDeleteCompressInfo(char *filePath);

extern cJSON *compressInfnToJson(CompressInfo *videoInfo);
