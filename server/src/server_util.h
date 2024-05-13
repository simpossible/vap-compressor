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
#include "cJSON.h"


extern char * getParamsFromRequest(struct mg_connection *conn, char *paramName);
extern cJSON * getPostFromRequest(struct mg_connection *conn);
extern int file_exists(char *filename);
extern bool string_end_with(const char *filePath, const char * suffix);
extern cJSON *videoInfnToJson(VideoInfo *videoInfo);
extern char * osJoinPath(const char* path1, const char* path2);
extern void copy_dir(const char* src_dir, const char* dest_dir);

#endif /* server_util_h */
