//
//  onOpenFile.c
//  VapCompressor
//
//  Created by 梁金锋 on 2024/11/28.
//


#include <stdio.h>
#include "civetweb.h"
#include "CJSON.h"
#include <stdlib.h>
#include "vap_parser.h"
#include "transcode.h"
#include "server_util.h"
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include "server_util.h"
extern char * OpenFinderForPath(char *path);

int onOpenFinderRequest(struct mg_connection *conn, void *ignored) {
    char *filePath = getParamsFromRequest(conn, "path");
    char *result = OpenFinderForPath(filePath);
    long len = strlen(result);
    mg_write(conn, result, len);
    return 200;
}
