//
//  string_util.c
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#include "string_util.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

bool isFileInnerPath(char *fileName) {
    return string_start_with(fileName, "__compress_");
}

bool string_start_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool string_end_with(const char *filePath, const char * suffix) {
    size_t len_str = strlen(filePath);
    size_t len_suffix = strlen(suffix);
    if(len_str < len_suffix)
        return false;
    return strncmp(filePath + len_str - len_suffix, suffix, len_suffix) == 0;
}


char **char_add_element(char **array, int *length, char *element) {
    array = realloc(array, (*length + 1) * sizeof(char *));
    array[*length] = malloc(strlen(element) + 1);
    strcpy(array[*length], element);
    (*length)++;
    return array;
}

char **char_remove_element(char **array, int *length, int index) {
    free(array[index]);
    for(int i = index; i < *length - 1; i++) {
        array[i] = array[i + 1];
    }
    array = realloc(array, (*length - 1) * sizeof(char *));
    (*length)--;
    return array;
}


char * tempVapPathFrom(const char *filePath) {
    char *filePathCopy = strdup(filePath);
    char *basePath = dirname(filePathCopy);
    unsigned long basePathLen = strlen(basePath);

    char *filePathCopy2 = strdup(filePath);
    char *baseName = basename(filePathCopy2);
    unsigned long baseNameLen = strlen(baseName);
    unsigned long outPutLen = basePathLen + baseNameLen + 13;
    char *outputPath = malloc(outPutLen);
    snprintf(outputPath, outPutLen, "%s/__compress_%s", basePath, baseName);
    free(filePathCopy);
    free(filePathCopy2);
    return outputPath;
}
