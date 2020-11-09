//
// Created by Godrick Crown on 2020/11/2.
//

#include "Audio.h"

Audio::Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j) {
    this->playState = playState;
    this->sampleRate = sampleRate;
    this->cb2j = cb2j;
    this->pSLProcessor = new SLProcessor(sampleRate);

    pthread_mutex_init(&this->mutexDecode, nullptr);

}

void *threadCallbackDecode(void *data) {
    Audio *instance = static_cast<Audio *>(data);
    instance->pSLProcessor->initialize();
    return 0;
}

void Audio::play() {
    if (this->playState && !this->playState->exit) {
        pthread_create(this->pThreadDecode, nullptr, threadCallbackDecode, this);
    }
}

int Audio::resampleAudio(void **pcmBuffer) {


    while (this->playState && !this->playState->exit) {

        if (this->playState->seek) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;
        }


        if (this->queue->getQueueSize() == 0) {//hasn't data

            if (!this->playState->load) {
                this->playState->load = true;
                this->cb2j->cb2j_MediaPlayer_Loading(WORK_THREAD, true);
            }
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;

        } else {                               //has data

            if (this->playState->load) {
                this->playState->load = false;
                this->cb2j->cb2j_MediaPlayer_Loading(WORK_THREAD, false);
            }

            this->pAVPacket = av_packet_alloc();

            if (this->queue->getAVPacket(this->pAVPacket) != 0) {
                av_packet_free(&this->pAVPacket);
                av_free(this->pAVPacket);
                this->pAVPacket = nullptr;
                continue;
            }

            //decode

            pthread_mutex_lock(&this->mutexDecode);

            ret = avcodec_send_packet(this->pAVCodecContext, this->pAVPacket);




            pthread_mutex_unlock(&this->mutexDecode);

            this->pAVFrame = av_frame_alloc();


        }


    }


    return 0;
}
