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
#include <stdlib.h>

typedef struct VapFileInfo {
    unsigned long long size;
    VideoInfo * video_info;
    cJSON *vap_info;
    char * path;
} VapFileInfo;


typedef struct {
    char *boxType;
    uint64_t start;
    uint64_t size;
    char *content;
} Mp4Box;

typedef struct {
    Mp4Box **boxes;
    uint64_t length;
    int state; //如果state=1那么需要删除这个文件
} BoxArray;

// 获取vapcJson
extern cJSON *getVapcInfo(char *filePath);
// 获取vapc字符串
char * getVapcContent(char *filePath);


extern cJSON * getVapFileInfoJson(char * filePath);
VapFileInfo* getFileInfoOfVap(char *filePath) ;
VapFileInfo* getVapFileInfoAllowNotVap(char *filePath);
int addVapcToMp4File(char * outPath, char *vapc);
BoxArray getVapBoxes(char *file_path);

#endif /* vap_parser_h */
