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
} CompressInfo;


extern CompressInfo* getCompressInfo(char *filePath);

extern void saveCompressInfo(char *filePath, CompressInfo *info);


extern cJSON *compressInfnToJson(CompressInfo *videoInfo);
