//
// Created by Godrick Crown on 2020/10/31.
//

#ifndef MEDIAPLAYER_FFMPEG_QUEUE_H
#define MEDIAPLAYER_FFMPEG_QUEUE_H

#include <queue>
#include <pthread.h>
#include "../controller/PlayState.h"
#include "../util/LogUtil.h"

extern "C" {
#include "libavcodec/avcodec.h"
};

class Queue {

public:
    std::queue<AVPacket *> qAVPacket;
    pthread_mutex_t mutexQueue;
    pthread_cond_t condQueue;
    PlayState *playState = nullptr;

public:
    Queue(PlayState *playState);

    ~Queue();

public:
    int putAVPacket(AVPacket *packet);

    int getAVPacket(AVPacket *packet);

    int getQueueSize();

    void clearAVPacketQueue();

    void noticeAVPackQueue();
};


#endif //MEDIAPLAYER_FFMPEG_QUEUE_H
