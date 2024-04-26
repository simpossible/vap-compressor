//
//  transcode.h
//  Vap
//
//  Created by 梁金锋 on 2024/4/23.
//

#ifndef transcode_h
#define transcode_h

typedef struct VideoInfo {
    int width;
    float height;
    int64_t duration;
    float bitrate;
} VideoInfo;

extern VideoInfo *getMp4Info(char * input);

#endif /* transcode_h */

