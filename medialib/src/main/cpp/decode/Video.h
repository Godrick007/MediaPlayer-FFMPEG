//
// Created by Godrick Crown on 2020/11/2.
//

#ifndef MEDIAPLAYER_FFMPEG_VIDEO_H
#define MEDIAPLAYER_FFMPEG_VIDEO_H


#include "../controller/PlayState.h"
#include "../Callback2Java.h"
#include "Audio.h"
#include "Queue.h"
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include "../egl/GLESRenderer.h"

extern "C" {
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/imgutils.h"
#include "../include/libswscale/swscale.h"
#include "../include/libavutil/common.h"
};


class Video {

public:
    PlayState *playState = nullptr;
    Callback2Java *cb2j = nullptr;

    int streamIndex = 0;
    double defaultDelayTime = 0;
    double delayTime = 0;

    Audio *audio = nullptr;

    AVRational timeBase;
    AVCodecParameters *pAVCodecParameters = nullptr;
    AVCodecContext *pAVCodecContext = nullptr;

    AVBSFContext *pAVBSFContext = nullptr;
    AVBitStreamFilter *pBsFilter = nullptr;


    Queue *queue = nullptr;

    pthread_t threadPlay;
    pthread_mutex_t mutexDecode;

    double clock;
    OldRenderer *renderer;

    double pts;
    double last_updated;

    SwsContext *pSwsContext = nullptr;

    void (*callbackYUVData)(int, int, void *, void *, void *);

    void (*callbackHWCodec)(void *, int);

    bool HWSupport = false;

//    FILE *fp_yuv;
//    const char *output = "/sdcard/output.yuv";

    bool frameVertexFixed = false;

public:
    Video(PlayState *playState, Callback2Java *cb2j);

    ~Video();

public:
    void play();

    double getFrameDiffTime(AVFrame *frame, AVPacket *pkt);

    double getDelayTime(double diff);

    void release();

    void setRenderer(OldRenderer *renderer);

    double fixTimeStamp(double current_pts);

    void dropFrames(double tar_pts);

    void setCallbackYUV(void (*pCallback)(int, int, void *, void *, void *));

    void setCallbackHW(void (*pCallback)(void *, int));

    void setHWSupport(bool s);

};


#endif //MEDIAPLAYER_FFMPEG_VIDEO_H
