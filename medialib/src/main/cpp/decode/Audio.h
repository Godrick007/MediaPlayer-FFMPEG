//
// Created by Godrick Crown on 2020/11/2.
//

#ifndef MEDIAPLAYER_FFMPEG_AUDIO_H
#define MEDIAPLAYER_FFMPEG_AUDIO_H


#include "PlayState.h"
#include "../Callback2Java.h"
#include "Queue.h"
#include <pthread.h>

extern "C" {
#include "../include/libavcodec/avcodec.h"
};

class Audio {

public:
    PlayState *playState = nullptr;
    Callback2Java *cb2j = nullptr;

    int sampleRate = 0;
    int streamIndex = 0;
    int duration = 0;

    Queue *queue = nullptr;
    AVCodecParameters *pAVCodecParameters = nullptr;
    AVCodecContext *pAVCodecContext = nullptr;
    AVPacket *pAVPacket = nullptr;
    AVFrame *pAVFrame = nullptr;
    AVRational timeBase;

    pthread_t threadPlay;


public:
    Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j);

    ~Audio();

public:
    void play();

    void pause();

    void stop();

    void release();
};


#endif //MEDIAPLAYER_FFMPEG_AUDIO_H
