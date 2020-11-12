//
// Created by Godrick Crown on 2020/10/29.
//

#ifndef MEDIAPLAYER_FFMPEG_LOGUTIL_H
#define MEDIAPLAYER_FFMPEG_LOGUTIL_H

#include <android/log.h>

#define LOG_DEBUG 1

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,__VA_ARGS__)


class LogUtil {

public:


};


#endif //MEDIAPLAYER_FFMPEG_LOGUTIL_H
