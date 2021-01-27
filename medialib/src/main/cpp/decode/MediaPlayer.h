//
// Created by Godrick Crown on 2020/10/31.
//

#ifndef MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H
#define MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H


#include <sys/types.h>
#include "../Callback2Java.h"
#include "../controller/PlayState.h"
#include "Video.h"
#include "Audio.h"
#include "../egl/GLESRenderer.h"
#include <pthread.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
};

class MediaPlayer {

public:
    Callback2Java *cb2j = nullptr;
    const char *url = nullptr;
    pthread_t threadInit;
    PlayState *playState = nullptr;
    AVFormatContext *pAVFormatContext = nullptr;
    OldRenderer *render = nullptr;


    pthread_mutex_t mutexInit;

    bool exit = false;
    bool isLiving = false;

    int duration = 0;

    pthread_mutex_t mutexSeek;

    Audio *audio = nullptr;
    Video *video = nullptr;
    const AVBitStreamFilter *pBsFilter = nullptr;

    pthread_t threadPlay;

    int ret;

public:

    MediaPlayer(PlayState *playState, Callback2Java *cb2j, OldRenderer *render);

    MediaPlayer(PlayState *playState, Callback2Java *cb2j, const char *url, OldRenderer *render, bool isLiving);

    ~MediaPlayer();

public:

    void setUrl(const char *url);

    void prepare();

    void setAudio(Audio *audio);

    void play();

    void pause();

    void resume();

    void stop();

    void release();

    void seek(int64_t second);

    void setVolume(int percent);

    void setChannel(int channel);

    void setSpeed(float speed);

    void setPitch(float pitch);

    int getSampleRate();

    void startStopRecord(bool start);


    void initialized();

    void startPlay();

    void setRenderer(OldRenderer *renderer);

private :

    int getAVCodecContext(AVCodecParameters *param, AVCodecContext **codecContext);

};


#endif //MEDIAPLAYER_FFMPEG_MEDIAPLAYER_H
