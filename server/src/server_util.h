//
//  server_util.h
//  vap-server
//
//  Created by 梁金锋 on 2024/4/26.
//

#ifndef server_util_h
#define server_util_h
#include <stdbool.h>
#include "civetweb.h"
#include "CJSON.h"
#include "transcode.h"
extern char * getParamsFromRequest(struct mg_connection *conn, char *paramName);
extern int file_exists(char *filename);
extern bool string_end_with(const char *filePath, const char * suffix);

extern cJSON *videoInfnToJson(VideoInfo *videoInfo);
extern char * osJoinPath(const char* path1, const char* path2);
extern bool isFileInnerPath(char *fileName);
bool string_start_with(const char *str, const char *prefix);
extern char **char_add_element(char **array, int *length, char *element);
extern char **char_remove_element(char **array, int *length, int index);
#endif /* server_util_h */
