//
// Created by Godrick Crown on 2020/10/31.
//

#ifndef MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H
#define MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H


#include <sys/types.h>
#include "../Callback2Java.h"
#include "PlayState.h"

extern "C" {
#include <libavformat/avformat.h>
};

class MediaPlayer {

public:
    Callback2Java *cb2j = nullptr;
    const char *url = nullptr;
    pthread_t threadDecode;
    PlayState *playState = nullptr;
    AVFormatContext *pAVFormatContext;

public:
    MediaPlayer(PlayState *playState, Callback2Java *cb2j, const char *url);

    ~MediaPlayer();

public:
    void prepare();
    void decodeFFMPEGThread();
    void setAudio();
};


#endif //MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H
