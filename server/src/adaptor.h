//
//  Header.h
//  Vap
//
//  Created by 梁金锋 on 2024/5/14.
//

#ifndef Header_h
#define Header_h

extern void setVapServerWorkSpacePath(const char * path);
extern void setAppDragFiles(const char * files_json_str);
extern void setVapHtmlResourcePath(const char * path);
extern int vapServerStart(void);
#endif /* Header_h */
