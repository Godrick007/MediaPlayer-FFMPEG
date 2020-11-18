//
// Created by Godrick Crown on 2020/11/2.
//

#ifndef MEDIAPLAYER_FFMPEG_AUDIO_H
#define MEDIAPLAYER_FFMPEG_AUDIO_H


#include "PlayState.h"
#include "../Callback2Java.h"
#include "Queue.h"
#include "SLProcessor.h"
#include "../soundtouch/SoundTouch.h"
#include <pthread.h>

extern "C" {
#include "../include/libavutil/channel_layout.h"
#include "../include/libavutil/frame.h"
#include "../include/libavutil/mem.h"
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/time.h"
#include "../include/libswresample/swresample.h"
};

using namespace soundtouch;

class SLProcessor;

class Audio {

public:
    PlayState *playState = nullptr;
    Callback2Java *cb2j = nullptr;

    int sampleRate = 0;
    int streamIndex = 0;
    int duration = 0;

    SoundTouch *pSoundTouch = nullptr;
    float speed = 1.0f;
    float pitch = 1.0f;


    Queue *queue = nullptr;
    AVCodecParameters *pAVCodecParameters = nullptr;
    AVCodecContext *pAVCodecContext = nullptr;
    AVPacket *pAVPacket = nullptr;
    AVFrame *pAVFrame = nullptr;
    AVRational timeBase;

    SwrContext *pSwrContext = nullptr;

    SLProcessor *pSLProcessor = nullptr;

    pthread_t threadDecode;

    pthread_mutex_t mutexDecode;

    int ret;
    int nb;
    long dataSize;
    uint8_t *buffer_u8;
    uint16_t *buffer_u16;
    int num;
    int nSample;

    SAMPLETYPE *pSoundTouchBuffer = nullptr;

    bool finish = false;
    bool isLiving = false;

    double nowTime;
    double clock;
    long lastTime;

public:
    Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j, bool isLiving = false);

    ~Audio();

    int resampleAudio(void **pcmBuffer);

    int decodeAudio(void **pcmBuffer);

    int getSoundTouchData(void **sampleBuffer);

public:
    void play();

    void pause();

    void resume();

    void stop();

    void setSpeed(float speed);

    void setPitch(float pitch);

    void release();

    void setVolume(int percent);
};


#endif //MEDIAPLAYER_FFMPEG_AUDIO_H
