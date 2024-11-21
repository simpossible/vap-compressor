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
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <CommonCrypto/CommonDigest.h>
#endif
static char * __set_app_cache_dir = NULL;
char* get_md5_string(const char *str) ;
char* concatenate(const char* str1, const char* str2);
#ifdef _WINDOWS
char* basename(char* path) {
    char* base = strrchr(path, '\\');
    return base ? base + 1 : path;
}

char* dirname(char* path) {
    char* dir = strrchr(path, '\\');
    if (dir) *dir = '\0';  // Cut the path at the last backslash
    return path;
}
#else
#include <libgen.h>
#endif

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
    char *basePath = __set_app_cache_dir;
    unsigned long basePathLen = strlen(basePath);
    
    char * md5Name = get_md5_string(filePath);
    unsigned long md5Length = strlen(md5Name);

    char *filePathCopy2 = strdup(filePath);
    char *baseName = basename(filePathCopy2);
    unsigned long baseNameLen = strlen(baseName);
    unsigned long outPutLen = basePathLen + baseNameLen + 14 + md5Length;
    char *outputPath = malloc(outPutLen);
    
    char * md5DirTemp = concatenate(basePath, "/");
    char * md5Dir = concatenate(md5DirTemp, md5Name);
    free(md5DirTemp);
    checkAndCreateDirectory(md5Dir);
    free(md5Dir);
    
#ifdef _WINDOWS
    snprintf(outputPath, outPutLen, "%s\%s\__compress_%s", basePath, baseName);
#else
    snprintf(outputPath, outPutLen, "%s/%s/__compress_%s", basePath,md5Name, baseName);
#endif
    free(filePathCopy);
    free(filePathCopy2);
    return outputPath;
}


extern void set_app_cache_dir(const char * path) {
    __set_app_cache_dir = strdup(path);
}


char* get_md5_string(const char *str) {
#ifdef __APPLE__
    unsigned char digest[CC_MD5_DIGEST_LENGTH];
    CC_MD5_CTX ctx;
    CC_MD5_Init(&ctx);
    CC_MD5_Update(&ctx, str, (CC_LONG)strlen(str));
    CC_MD5_Final(digest, &ctx);

    // Allocate memory for the MD5 string (32 characters + null terminator)
    char *md5_string = (char*)malloc(2 * CC_MD5_DIGEST_LENGTH + 1);
    if (md5_string == NULL) {
        return NULL; // Memory allocation failed
    }

    // Convert the MD5 hash to a string
    for (int i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
        sprintf(&md5_string[i * 2], "%02x", digest[i]);
    }

    return md5_string;
#endif
}

// Function to concatenate two strings
char* concatenate(const char* str1, const char* str2) {
    // Calculate the length of the new string
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t totalLen = len1 + len2 + 1; // +1 for the null terminator

    // Allocate memory for the new string
    char* result = (char*)malloc(totalLen * sizeof(char));
    if (result == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Copy the first string to the result
    strcpy(result, str1);

    // Concatenate the second string to the result
    strcat(result, str2);

    return result;
}


void checkAndCreateDirectory(const char *path) {
    struct stat st = {0};

    // Check if the directory exists
    if (stat(path, &st) == -1) {
        // Directory does not exist, create it
        if (mkdir(path, 0700) == 0) {
            printf("Directory created: %s\n", path);
        } else {
            perror("mkdir");
        }
    } else {
        printf("Directory already exists: %s\n", path);
    }
}
