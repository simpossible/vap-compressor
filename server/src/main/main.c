//
//  main.c
//  vap-server
//
//  Created by 梁金锋 on 2024/5/16.
//

#include "adaptor.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    setVapHtmlResourcePath("");
    vapServerStart();
    setAppDragFiles("");
    return 0;
}
