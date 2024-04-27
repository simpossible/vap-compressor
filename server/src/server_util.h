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
#include "string_util.h"
extern char * getParamsFromRequest(struct mg_connection *conn, char *paramName);
extern int file_exists(char *filename);
extern bool string_end_with(const char *filePath, const char * suffix);

extern cJSON *videoInfnToJson(VideoInfo *videoInfo);
extern char * osJoinPath(const char* path1, const char* path2);


#endif /* server_util_h */
