//
//  vap_parser.h
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#ifndef vap_parser_h
#define vap_parser_h
#include "CJSON.h"
#include "transcode.h"
#include "transcode.h"
typedef enum {
    CompressState_error = -1,
    CompressState_none = 0,
    CompressState_compressing = 1,
    CompressState_done = 2,
    CompressState_acceptting = 3,
    CompressState_quitting = 4
} CompressState;

typedef struct VapFileInfo {
    unsigned long long size;
    VideoInfo * video_info;
    cJSON *vap_info;
    char * path;
} VapFileInfo;

extern cJSON *getVapInfo(char *filePath);
VapFileInfo* getFileInfoOfVap(char *filePath) ;

#endif /* vap_parser_h */
