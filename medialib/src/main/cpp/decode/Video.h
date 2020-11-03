//
// Created by Godrick Crown on 2020/11/2.
//

#ifndef MEDIAPLAYER_FFMPEG_VIDEO_H
#define MEDIAPLAYER_FFMPEG_VIDEO_H


#include "PlayState.h"
#include "../Callback2Java.h"
#include "Audio.h"
#include "Queue.h"

extern "C" {
#include "../include/libavcodec/avcodec.h"
};

class Video {

public:
    PlayState *playState = nullptr;
    Callback2Java *cb2j = nullptr;

    int streamIndex = 0;
    float defaultDelayTime = 0;


    Audio *audio = nullptr;

    AVRational timeBase;
    AVCodecParameters *pAVCodecParameters = nullptr;
    AVCodecContext *pAVCodecContext = nullptr;

    Queue *queue = nullptr;

public:
    Video(PlayState *playState, Callback2Java *cb2j);

    ~Video();

public:
    void play();
};


#endif //MEDIAPLAYER_FFMPEG_VIDEO_H
