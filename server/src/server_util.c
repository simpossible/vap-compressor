//
//  server_util.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/26.
//


#ifndef server_util_c
#define server_util_c
#include "server_util.h"
#include "civetweb.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include "cJSON.h"

#endif


char * getParamsFromRequest(struct mg_connection *conn, char *paramName){
    const struct mg_request_info *ri = mg_get_request_info(conn);
    if (strcmp(ri->request_method, "GET") == 0) {
        /* ri->query_string 包含了 URL 中的查询字符串 */
        const char *query_string = ri->query_string;
        
        /* 使用 mg_get_var() 来解析查询字符串并获取参数值 */
        char * paramValue = (char *)malloc(100);
        memset(paramValue, 0, 100);
        size_t paramValueLen = 0;
        size_t length = strlen(query_string);
        int ret = mg_get_var(query_string, length, paramName, paramValue, 100);
        if (ret == 0) {
            /* 找到参数，paramValue 包含了参数的值 */
            printf("The value of paramName is: %.*s\n", (int)paramValueLen, paramValue);
            free(paramValue);
            return NULL;
        }
        return  paramValue;
    }
    return NULL;
}

cJSON * getPostFromRequest(struct mg_connection *conn) {
    char buf[1024];
    ssize_t n, total = 0;
    int ret;
    
    // 创建一个足够大的动态缓冲区，或者使用 realloc 动态增长
    char *data = malloc(2048); // 初始分配更大的内存
    if (data == NULL) {
        // 内存分配失败
        return NULL;
    }
    
    // 循环读取数据直到读取完毕
    do {
        n = mg_read(conn, buf, sizeof(buf));
        if (n <= 0) {
            free(data);
            return NULL; // 读取失败
        }
        
        // 检查是否需要重新分配内存
        if (total + n >= (ssize_t)strlen(data)) {
            char *tmp = realloc(data, total + n + 1);
            if (tmp == NULL) {
                free(data);
                return NULL; // 内存重新分配失败
            }
            data = tmp;
        }
        
        // 将读取的数据追加到动态缓冲区
        memcpy(data + total, buf, n);
        total += n;
    } while (n == sizeof(buf));

    char * rawData = malloc(total + 1);
    memcpy(rawData, data, total);
    rawData[total] = '\0';
    // 数据末尾添加空字符;
    cJSON *result = cJSON_Parse(rawData);
    free(rawData);
    free(data);    
    // 返回成功
    return result;
}


int file_exists(char *filename) {
    return access(filename, F_OK);
}





cJSON *videoInfnToJson(VideoInfo *videoInfo) {
    cJSON *json =  cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "width", videoInfo->width);
    cJSON_AddNumberToObject(json, "height", videoInfo->height);
    cJSON_AddNumberToObject(json, "duration_ts", videoInfo->duration);
    cJSON_AddStringToObject(json, "codec_name", videoInfo->codec_name);
    cJSON_AddNumberToObject(json, "bit_rate", videoInfo->bitrate);
    return json;
}


char * osJoinPath(const char* path1, const char* path2) {
    size_t targetLen = strlen(path1) + strlen(path2) + 1;
    if (path1[strlen(path1) - 1] != '/') {
        targetLen = targetLen + 1;
    }
    char * destination = (char *)malloc(targetLen);
    if(path1[strlen(path1) - 1] == '/') {
        snprintf(destination, targetLen, "%s%s", path1, path2);
    } else {
        snprintf(destination, targetLen, "%s/%s", path1, path2);
    }
    return destination;
    
}





