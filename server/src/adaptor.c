#ifndef HAS__INCLUDE__ADAPTOR
#define HAS__INCLUDE__ADAPTOR
#include "adaptor.h"
#include <stdlib.h>
#include <string.h>
#include "server_util.h"
#include "vap-server.h"
#include <pthread.h>
#include <stdio.h>
#endif



static char * __vap_drag_files = NULL;

void setAppDragFiles(const char * files_json_str) {
    if (__vap_drag_files) {
        free(__vap_drag_files);
        __vap_drag_files = NULL;
    }
    __vap_drag_files = strdup(files_json_str);
}

char * getAppDragFiles(void) {
    return strdup(__vap_drag_files);
}


static char * __vap_html_resource_path = NULL;

void setVapHtmlResourcePath(const char * path) {
    if (__vap_html_resource_path) {
        free(__vap_html_resource_path);
    }
    __vap_html_resource_path = strdup(path);
}

void* my_thread_lanuch_func(void* arg) {
    startVapServer(__vap_html_resource_path);
    return NULL;
}


int vapServerStart(void) {
    // __vap_html_resource_path 是否存在，不存在就返回-1
    if (__vap_html_resource_path == NULL) {
        return -1;
    }
    //判断文件夹__vap_html_resource_path是否存在
    if (file_exists(__vap_html_resource_path) == -1) {
        return -1;
    }
    // 开启一个线程执行startVapServer
    startVapServer(__vap_html_resource_path);
    return 0;
    
}
