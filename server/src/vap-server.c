//
//  vap-server.c
//  vap-server
//
//  Created by 梁金锋 on 2024/4/18.
//

#include "vap-server.h"
#include "civetweb.h"
#include <string.h>
#include "server_util.h"
#include "dictionary.h"
#include <stdint.h>
#include "CJSON.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include "vap_parser.h"
#include "transcode.h"
#include <dirent.h>
#include <stdlib.h>

static struct Dictionary * compress_dic = NULL;
extern int onFileRequest(struct mg_connection *conn, void *ignored);
extern int onVapFileListRequest(struct mg_connection *conn, void *ignored);
extern int onVapInfoRequest(struct mg_connection *conn, void *ignored);
extern int onDownloadRequest(struct mg_connection *conn, void *ignored);
extern int onVapJsonRequest(struct mg_connection *conn, void *ignored);
extern int onCompressInfoRequest(struct mg_connection *conn, void *ignored);
extern int onStartCompressRequest(struct mg_connection *conn, void *ignored);
extern int onClearFilesRequest(struct mg_connection *conn, void *ignored);
extern int onAcceptCompressnRequest(struct mg_connection *conn, void *ignored);
extern int onQuitCompressnRequest(struct mg_connection *conn, void *ignored);
extern int onDragRequest(struct mg_connection *conn, void *ignored);




void startVapServer(const char *workspacePath) {
    
    // 初始化压缩的字典
    compress_dic = dicCreate();
    
    /* Initialize CivetWeb library without OpenSSL/TLS support. */
    mg_init_library(0);
    /* Start the server using the advanced API. */
    struct mg_callbacks callbacks = {0};
    void *user_data = NULL;
    
    struct mg_init_data mg_start_init_data = {0};
    mg_start_init_data.callbacks = &callbacks;
    mg_start_init_data.user_data = user_data;
    const char *documentPath = workspacePath;
    const char *options[] = {
        "document_root", documentPath,
        "listening_ports", "3000",
        NULL
    };
    mg_start_init_data.configuration_options = options;
    
    struct mg_error_data mg_start_error_data = {0};
    char errtxtbuf[256] = {0};
    mg_start_error_data.text = errtxtbuf;
    mg_start_error_data.text_buffer_size = sizeof(errtxtbuf);
    
    struct mg_context *ctx =
    mg_start2(&mg_start_init_data, &mg_start_error_data);
    if (!ctx) {
        fprintf(stderr, "Cannot start server: %s\n", errtxtbuf);
        return;
    }
    mg_set_request_handler(ctx, "/file", onFileRequest, "file");
    mg_set_request_handler(ctx, "/vap-file-list", onVapFileListRequest, "onVapFileListRequest");
    mg_set_request_handler(ctx, "/vap-info", onVapInfoRequest, "vap-info");
    mg_set_request_handler(ctx, "/download", onDownloadRequest, "download");
    mg_set_request_handler(ctx, "/vap-json", onVapJsonRequest, "vap-json");
    mg_set_request_handler(ctx, "/compress-info", onCompressInfoRequest, "compress-info");
    mg_set_request_handler(ctx, "/start-compress", onStartCompressRequest, "compress-info");
    mg_set_request_handler(ctx, "/quit-compress", onQuitCompressnRequest, "quit-compress");
    mg_set_request_handler(ctx, "/accept-compress", onAcceptCompressnRequest, "accept-compress");
    mg_set_request_handler(ctx, "/clear-files", onClearFilesRequest, "clear-files");
    mg_set_request_handler(ctx, "/on-drag", onDragRequest, "on-drag");
    
    
}
