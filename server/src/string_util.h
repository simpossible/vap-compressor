//
//  string_util.h
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#ifndef string_util_h
#define string_util_h

#include <stdio.h>
#include <stdbool.h>
extern bool isFileInnerPath(char *fileName);
bool string_start_with(const char *str, const char *prefix);
bool string_end_with(const char *filePath, const char * suffix);
extern char **char_add_element(char **array, int *length, char *element);
extern char **char_remove_element(char **array, int *length, int index);
extern char * tempVapPathFrom(const char *filePath);
#endif /* string_util_h */
