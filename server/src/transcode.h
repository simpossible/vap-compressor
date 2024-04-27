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
    long long duration;
    float bitrate;
    char * codec_name;
} VideoInfo;

extern VideoInfo *getMp4Info(char * input);
extern int av_compress_video(char* inputPath, char *outputPath);

#endif /* transcode_h */

