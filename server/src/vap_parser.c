//
//  vap_parser.c
//  Vap
//
//  Created by 梁金锋 on 2024/4/27.
//

#include "vap_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "CJSON.h"
#include "string_util.h"
#include <sys/stat.h>


typedef struct {
    char *boxType;
    uint64_t start;
    uint64_t size;
    char *content;
} Mp4Box;

typedef struct {
    Mp4Box **boxes;
    uint64_t length;
} BoxArray;

BoxArray getVapBoxes(char *filePath);

Mp4Box* createMp4Box(char *boxType, uint64_t start, uint64_t size, char *content) {
    Mp4Box *newBox = malloc(sizeof(Mp4Box));
    newBox->boxType = boxType;
    newBox->start = start;
    newBox->size = size;
    newBox->content = content;
    return newBox;
}

// This function is a placeholder. You'll need to implement it according to your needs.
uint8_t* readFromFile(FILE *file, uint64_t position, uint64_t length) {
    // Check if the requested position is beyond the end of the file
    fseek(file, 0, SEEK_END);
    uint64_t fileSize = ftell(file);
    if (position >= fileSize) {
        printf("Position is beyond the end of the file\n");
        return NULL;
    }

    // Check if the requested length goes beyond the end of the file
    if (position + length > fileSize) {
        printf("Requested length goes beyond the end of the file\n");
        return NULL;
    }

    uint8_t *buffer = malloc(length);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    fseek(file, position, SEEK_SET);
    fread(buffer, sizeof(uint8_t), length, file);

    return buffer;
}

char* readCharFromFile(FILE *file, uint64_t position, uint64_t length) {
    fseek(file, 0, SEEK_END);
    uint64_t fileSize = ftell(file);
    if (position >= fileSize) {
        printf("Position is beyond the end of the file\n");
        return NULL;
    }

    // Check if the requested length goes beyond the end of the file
    if (position + length > fileSize) {
        printf("Requested length goes beyond the end of the file\n");
        return NULL;
    }
    char *buffer = malloc(length + 1);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    fseek(file, position, SEEK_SET);
    fread(buffer, sizeof(char), length, file);
    buffer[length] = '\0'; // Null-terminate the string

    return buffer;
}

uint32_t readUInt32BE(uint8_t *buffer) {
    return ((uint32_t)buffer[0] << 24) |
           ((uint32_t)buffer[1] << 16) |
           ((uint32_t)buffer[2] << 8)  |
           (uint32_t)buffer[3];
}

uint64_t readUInt64BE(uint8_t *buffer) {
    return ((uint64_t)buffer[0] << 56) |
           ((uint64_t)buffer[1] << 48) |
           ((uint64_t)buffer[2] << 40) |
           ((uint64_t)buffer[3] << 32) |
           ((uint64_t)buffer[4] << 24) |
           ((uint64_t)buffer[5] << 16) |
           ((uint64_t)buffer[6] << 8)  |
           (uint64_t)buffer[7];
}

char* convertToString(uint8_t *buffer, int length) {
    char *str = malloc(length + 1);
    if (str == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    memcpy(str, buffer, length);
    str[length] = '\0';  // Null-terminate the string

    return str;
}

BoxArray getVapBoxes(char *file_path) {
    printf("start parse mp4\n");
    FILE *fd = fopen(file_path, "r");
    Mp4Box **allBoxes = malloc(sizeof(Mp4Box*) * 100); // Assuming max 100 boxes.
    uint64_t position = 0;
    uint64_t boxCount = 0;
    while (1) {
        uint64_t boxStart = position;
        uint8_t *sizeReadBuffer = readFromFile(fd, position, 4);
        uint64_t headerSize = 4;
        if (sizeReadBuffer == NULL) {
            printf("read file end or error\n");
            break;
        }
        position += 4;
        uint64_t boxSize = readUInt32BE(sizeReadBuffer);
        free(sizeReadBuffer);
        printf("boxSize: %lld\n", boxSize);

        uint8_t *boxTypeReadBuffer = readFromFile(fd, position, 4);
        headerSize += 4;
        if (boxTypeReadBuffer == NULL) {
            printf("read file error - 1\n");
            break;
        }
        char *boxType = convertToString(boxTypeReadBuffer, 4);
        free(boxTypeReadBuffer);
        position += 4;

        if (boxSize == 1) {
            uint8_t *largeSizeReadBuffer = readFromFile(fd, position, 8);
            if (largeSizeReadBuffer == NULL) {
                printf("read file error - 2\n");
                break;
            }
            position += 8;
            boxSize = readUInt64BE(largeSizeReadBuffer);
            free(largeSizeReadBuffer);
            headerSize += 8;
        }
        char *boxContent = "";
        if (strcmp(boxType, "vapc") == 0) {
            printf("box size is %lld, headersize is %lld, position is %lld\n", boxSize, headerSize, position);
            uint64_t vapcLen = boxSize - headerSize;
            char *contentReadBuffer = readCharFromFile(fd, position, vapcLen);
            printf("vaplen is %lld, content read is %lu\n", vapcLen, strlen(contentReadBuffer));
            position += vapcLen;
            if (contentReadBuffer == NULL) {
                printf("read file error - 3\n");
                break;
            }
            boxContent = contentReadBuffer;
        }
        Mp4Box *box = createMp4Box(boxType, boxStart, boxSize, boxContent);
        allBoxes[boxCount++] = box;
        position = boxStart + boxSize;
    }
    BoxArray boxArray = {
        .boxes = allBoxes,
        .length = boxCount
    };
    return boxArray;
}

cJSON *getVapInfo(char *filePath) {
    BoxArray array = getVapBoxes(filePath);
    for (int i = 0; i < array.length; i++) {
        Mp4Box *box = array.boxes[i];
        if (strcmp(box->boxType, "vapc") == 0) {
            cJSON *vapJson = cJSON_Parse(box->content);
            return vapJson;
        }
    }
    return NULL;
}


VapFileInfo* getFileInfoOfVap(char *filePath) {
    if (!string_end_with(filePath, ".mp4")){
        return NULL;
    }
    struct stat fileStat;
    if(stat(filePath, &fileStat) < 0) {
        return NULL;
    }
    cJSON *vapInfo = getVapInfo(filePath);
    if (vapInfo == NULL) {
        return NULL;
    }
    
    VapFileInfo *info = malloc(sizeof(VapFileInfo));
    info->size = fileStat.st_size;
    info->vap_info = vapInfo;
    info->video_info = getMp4Info(filePath);
    info->path = filePath;
    return info;
    
}

