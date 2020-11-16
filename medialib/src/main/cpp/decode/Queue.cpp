//
// Created by Godrick Crown on 2020/10/31.
//

#include "Queue.h"

Queue::Queue(PlayState *playState) {
    this->playState = playState;
    pthread_mutex_init(&mutexQueue, nullptr);
    pthread_cond_init(&condQueue, nullptr);
}

Queue::~Queue() {
    clearAVPacketQueue();
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
}

int Queue::putAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexQueue);
    qAVPacket.push(packet);
    pthread_cond_signal(&condQueue);
    pthread_mutex_unlock(&mutexQueue);
    return 0;
}

int Queue::getAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexQueue);
    while (playState == nullptr || !playState->exit) {
        if (qAVPacket.size() > 0) {
            AVPacket *pkt = qAVPacket.front();
            if (av_packet_ref(packet, pkt) == 0) {
                qAVPacket.pop();
            }
            av_packet_free(&pkt);
            av_free(pkt);
            pkt = nullptr;
            break;
        } else {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
    }
    pthread_mutex_unlock(&mutexQueue);
    return 0;
}

int Queue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexQueue);
    size = qAVPacket.size();
    pthread_mutex_unlock(&mutexQueue);
    return size;
}

void Queue::clearAVPacketQueue() {
    pthread_mutex_lock(&mutexQueue);
    while (qAVPacket.empty()) {
        AVPacket *pkt = qAVPacket.front();
        qAVPacket.pop();
        av_packet_free(&pkt);
        av_free(pkt);
        pkt = nullptr;
        break;
    }
    pthread_mutex_unlock(&mutexQueue);
}

void Queue::noticeAVPackQueue() {
    pthread_cond_signal(&condQueue);
}
