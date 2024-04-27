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


typedef struct VapFileInfo {
    unsigned long long size;
    VideoInfo * video_info;
    cJSON *vap_info;
    char * path;
} VapFileInfo;

extern cJSON *getVapInfo(char *filePath);
extern cJSON * getVapFileInfoJson(char * filePath);
VapFileInfo* getFileInfoOfVap(char *filePath) ;

#endif /* vap_parser_h */
