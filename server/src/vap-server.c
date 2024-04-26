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
#include "hashmap.h"





static int handleHellowWord(struct mg_connection *conn, void *ignored)
{
    printf("hahaha");
    const char *msg = "Hello world";
    unsigned long len = (unsigned long)strlen(msg);
    char *test = getParamsFromRequest(conn, "test");
    if (test != NULL){
        printf("a");
    }
    mg_send_http_ok(conn, "text/plain", len);
    
    mg_write(conn, msg, len);
    
    return 200; /* HTTP state 200 = OK */
}



static int onFileRequest(struct mg_connection *conn, void *ignored) {
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char param[100];
   
    
    return 1;
}



void startVapServer(void) {
    /* Initialize CivetWeb library without OpenSSL/TLS support. */
    mg_init_library(0);
    
    /* Start the server using the advanced API. */
    struct mg_callbacks callbacks = {0};
    void *user_data = NULL;
    
    struct mg_init_data mg_start_init_data = {0};
    mg_start_init_data.callbacks = &callbacks;
    mg_start_init_data.user_data = user_data;
    const char *documentPath = vapServerWorkSpacePath();
    const char *options[] = {
        "document_root", documentPath,
        "listening_ports", "8070",
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
    mg_set_request_handler(ctx, "/hello", handleHellowWord, "Hello world");
    
}
