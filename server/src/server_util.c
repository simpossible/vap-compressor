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


int file_exists(char *filename) {
    return access(filename, F_OK);
    return 0;
}


bool string_end_with(const char *filePath, const char * suffix) {
    size_t len_str = strlen(filePath);
    size_t len_suffix = strlen(suffix);
    if(len_str < len_suffix)
        return false;
    return strncmp(filePath + len_str - len_suffix, suffix, len_suffix) == 0;
}

bool string_start_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
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


bool isFileInnerPath(char *fileName) {
    return string_start_with(fileName, "__compress_");
}


char **char_add_element(char **array, int *length, char *element) {
    array = realloc(array, (*length + 1) * sizeof(char *));
    array[*length] = malloc(strlen(element) + 1);
    strcpy(array[*length], element);
    (*length)++;
    return array;
}

char **char_remove_element(char **array, int *length, int index) {
    free(array[index]);
    for(int i = index; i < *length - 1; i++) {
        array[i] = array[i + 1];
    }
    array = realloc(array, (*length - 1) * sizeof(char *));
    (*length)--;
    return array;
}
