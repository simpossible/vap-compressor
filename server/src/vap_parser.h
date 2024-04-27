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

// 获取vapcJson
extern cJSON *getVapcInfo(char *filePath);
// 获取vapc字符串
char * getVapcContent(char *filePath);


extern cJSON * getVapFileInfoJson(char * filePath);
VapFileInfo* getFileInfoOfVap(char *filePath) ;
VapFileInfo* getVapFileInfoAllowNotVap(char *filePath);
int addVapcToMp4File(char * outPath, char *vapc);

#endif /* vap_parser_h */
