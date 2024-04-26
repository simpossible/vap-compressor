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
