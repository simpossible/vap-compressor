//
//  server_util.h
//  vap-server
//
//  Created by 梁金锋 on 2024/4/26.
//

#ifndef server_util_h
#define server_util_h
#include "civetweb.h"
extern char * getParamsFromRequest(struct mg_connection *conn, char *paramName);

#endif /* server_util_h */
