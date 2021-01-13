//
// Created by Godrick Crown on 2020/11/2.
//

#include "Audio.h"

Audio::Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j, bool isLiving) {
    this->playState = playState;
    this->sampleRate = sampleRate;
    this->cb2j = cb2j;
    this->pSLProcessor = new SLProcessor(sampleRate, this);
    this->queue = new Queue(playState);
    this->buffer_u8 = static_cast<uint8_t *>(av_malloc(sampleRate * 2 * 2 * 2));
    this->buffer_u16 = static_cast<uint16_t *>(av_malloc(sampleRate * 2 * 2 * 2));
    this->pSoundTouchBuffer = static_cast<SAMPLETYPE *>(malloc(sampleRate * 2 * 2 * 2));
//    this->isLiving = isLiving;





    this->isLiving = false;
    if (isLiving) {

    } else {
        this->pSoundTouch = new SoundTouch();
        this->pSoundTouch->setSampleRate(sampleRate);
        this->pSoundTouch->setChannels(2);
        this->pSoundTouch->setPitch(this->pitch);
        this->pSoundTouch->setTempo(this->speed);
    }
    pthread_mutex_init(&this->mutexDecode, nullptr);

}

void *threadCallbackDecode(void *data) {
    Audio *instance = static_cast<Audio *>(data);
    instance->pSLProcessor->initialize();
    return 0;
}

void Audio::play() {
    if (this->playState != nullptr && !this->playState->exit) {
        pthread_create(&this->threadDecode, nullptr, threadCallbackDecode, this);
    }
}

int Audio::resampleAudio(void **pcmBuffer) {


    while (this->playState != nullptr && !this->playState->exit) {

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
        }

        this->pAVPacket = av_packet_alloc();

        if (this->queue->getAVPacket(this->pAVPacket) != 0) {
            av_packet_free(&this->pAVPacket);
            av_free(this->pAVPacket);
            this->pAVPacket = nullptr;
            continue;
        }



        //resample

        pthread_mutex_lock(&this->mutexDecode);

        ret = avcodec_send_packet(this->pAVCodecContext, this->pAVPacket);

        if (ret != 0) {
            av_packet_free(&this->pAVPacket);
            av_free(this->pAVPacket);
            this->pAVPacket = nullptr;
            pthread_mutex_unlock(&this->mutexDecode);
            continue;
        }


        this->pAVFrame = av_frame_alloc();

        ret = avcodec_receive_frame(this->pAVCodecContext, this->pAVFrame);

        if (ret != 0) {
            av_frame_free(&this->pAVFrame);
            av_free(this->pAVFrame);
            this->pAVFrame = nullptr;
            av_packet_free(&this->pAVPacket);
            av_free(this->pAVPacket);
            this->pAVPacket = nullptr;
            pthread_mutex_unlock(&this->mutexDecode);
            continue;
        } else {

            //resample
            if (this->pAVFrame->channels > 0 && this->pAVFrame->channel_layout == 0) {
                //according to channels set channel layout
                this->pAVFrame->channel_layout = av_get_default_channel_layout(
                        this->pAVFrame->channels);
            } else {
                //exception about get channels
                this->pAVFrame->channels = av_get_channel_layout_nb_channels(
                        this->pAVFrame->channel_layout);
            }


            //resample
            if (this->pSwrContext == nullptr) {
                pSwrContext = swr_alloc_set_opts(
                        nullptr,
                        AV_CH_LAYOUT_STEREO,
                        AV_SAMPLE_FMT_S16,
                        this->pAVFrame->sample_rate,
                        this->pAVFrame->channel_layout,
                        (AVSampleFormat)
                                this->pAVFrame->format,
                        this->pAVFrame->sample_rate,
                        0,
                        nullptr
                );

                if (pSwrContext == nullptr || swr_init(pSwrContext) < 0) {
                    av_frame_free(&this->pAVFrame);
                    av_free(this->pAVFrame);
                    this->pAVFrame = nullptr;
                    av_packet_free(&this->pAVPacket);
                    av_free(this->pAVPacket);
                    this->pAVPacket = nullptr;
                    if (pSwrContext != nullptr) {
                        swr_free(&pSwrContext);
                    }
                    pSwrContext = nullptr;
                    pthread_mutex_unlock(&this->mutexDecode);
                    continue;
                }
            }


//            int dst_nb_samples = av_rescale_rnd(
//                    swr_get_delay(pSwrContext, pAVFrame->sample_rate) + pAVFrame->nb_samples,
//                    pAVFrame->sample_rate, pAVFrame->sample_rate, AVRounding(1));

//            int dst_nb_samples = sizeof(buffer_u8) / (2 * 2);
//            pAVFrame->linesize[1] = pAVFrame->linesize[0];

            this->nb = swr_convert(
                    pSwrContext,
                    &this->buffer_u8,
                    this->pAVFrame->nb_samples,
                    (const uint8_t **) (this->pAVFrame->data),
                    this->pAVFrame->nb_samples
            );
//            *pcmBuffer = buffer_u8;

//            for (int i = 0; i < dataSize / 2 + 1; i++) {
//                pSoundTouchBuffer[i] = (buffer_u8[i * 2] | buffer_u8[i * 2 + 1] << 8);
//            }



//
            int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
//
            this->dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
//
            this->nowTime = this->pAVFrame->pts * av_q2d(this->timeBase);

            if (this->nowTime < this->clock) {
                this->nowTime = this->clock;
            }
            this->clock = this->nowTime;


            this->audio_clock =
                    pAVFrame->pts + (double) pAVFrame->nb_samples / pAVFrame->sample_rate;
            this->pts = pAVFrame->pts * av_q2d(timeBase);


            av_frame_free(&this->pAVFrame);
            av_free(this->pAVFrame);
            this->pAVFrame = nullptr;
            av_packet_free(&this->pAVPacket);
            av_free(this->pAVPacket);
            this->pAVPacket = nullptr;


            if (pSoundTouch == nullptr) {

                if (dataSize > 0) {

                    pSoundTouch->putSamples(pSoundTouchBuffer, nb);
                    nSample = pSoundTouch->receiveSamples(pSoundTouchBuffer, dataSize / 4);
                } else {
                    pSoundTouch->clear();
                }

                if (nSample == 0) {
                    pthread_mutex_unlock(&this->mutexDecode);
                    continue;
                } else {
                    if (*buffer_u8 == 0) {
                        nSample = pSoundTouch->receiveSamples(pSoundTouchBuffer, dataSize / 4);
                        if (nSample == 0) {
                            pthread_mutex_unlock(&this->mutexDecode);
                            continue;
                        }
                    }
                }
                *pcmBuffer = pSoundTouchBuffer;
//                dataSize *= 4;

            } else {
                *pcmBuffer = this->buffer_u8;
            }
            pthread_mutex_unlock(&this->mutexDecode);
            break;
        }
    }
    return this->dataSize;
}

int Audio::getSoundTouchData(void **sampleBuffer) {

    while (this->playState != nullptr && !this->playState->exit) {

        uint8_t *outBuffer = nullptr;
        if (this->finish) {
            finish = false;
            dataSize = resampleAudio(reinterpret_cast<void **>(&outBuffer));

            if (dataSize > 0) {

                for (int i = 0; i < dataSize / 2 + 1; i++) {
                    pSoundTouchBuffer[i] = (outBuffer[i * 2] | outBuffer[i * 2 + 1] << 8);
                }

                this->pSoundTouch->putSamples(pSoundTouchBuffer, nb);
                this->num = this->pSoundTouch->receiveSamples(
                        pSoundTouchBuffer, dataSize / 4);

            } else {
                this->pSoundTouch->flush();
            }
        }

        if (num == 0) {
            finish = true;
            continue;
        } else {

            if (outBuffer == nullptr) {
                this->num = this->pSoundTouch->receiveSamples(
                        pSoundTouchBuffer, dataSize / 4);
                if (num == 0) {
                    finish = true;
                    continue;
                }
            }
            *sampleBuffer = pSoundTouchBuffer;
            return num;
        }


    }
    return 0;
}


void Audio::setSpeed(float speed) {
    if (this->pSoundTouch != nullptr) {
        this->pSoundTouch->setTempo(speed);
    }
}

void Audio::setPitch(float pitch) {
    if (this->pSoundTouch != nullptr) {
        this->pSoundTouch->setPitch(pitch);
    }
}

void Audio::release() {

    if (queue != nullptr) {
        queue->noticeAVPackQueue();
    }

    pthread_join(threadDecode, nullptr);

    if (queue != nullptr) {
        delete queue;
        queue = nullptr;
    }

    if (pSoundTouch != nullptr) {
        delete pSoundTouch;
        pSoundTouch = nullptr;
    }

    if (pSLProcessor != nullptr) {
        delete pSLProcessor;
        pSLProcessor = nullptr;
    }

    if (playState != nullptr) {
        playState = nullptr;
    }

    if (cb2j != nullptr) {
        cb2j = nullptr;
    }

    if (pAVCodecContext != nullptr) {
        pthread_mutex_lock(&mutexDecode);
        avcodec_close(pAVCodecContext);
        avcodec_free_context(&pAVCodecContext);
        pAVCodecContext = nullptr;
        pthread_mutex_unlock(&mutexDecode);
    }

    if (buffer_u8 != nullptr) {
//        av_free(buffer_u8);
        buffer_u8 = nullptr;
    }

    if (buffer_u16 != nullptr) {
        av_free(buffer_u16);
        buffer_u16 = nullptr;
    }

    if (pSoundTouchBuffer != nullptr) {
//        av_free(pSoundTouchBuffer);
        pSoundTouchBuffer = nullptr;
    }

}

void Audio::stop() {
    if (pSLProcessor != nullptr) {
        pSLProcessor->stop();
    }
}

void Audio::pause() {
    if (pSLProcessor != nullptr) {
        pSLProcessor->pause();
    }
}

void Audio::resume() {
    if (pSLProcessor != nullptr) {
        pSLProcessor->resume();
    }
}

int Audio::decodeAudio(void **pcmBuffer) {


    return 0;
}

Audio::~Audio() {
    release();
    pthread_mutex_destroy(&mutexDecode);
}

void Audio::setVolume(int percent) {
    if (pSLProcessor != nullptr) {
        pSLProcessor->setVolume(percent);
    }
}

void Audio::setClock(double pts, double last_updated, double serial) {

    LOGI("clock ", "pts is %lf , last_update is %lf", pts, last_updated);

    this->pts = pts;
    this->last_updated = last_updated;
    this->drift = pts - last_updated;
    this->serial = serial;
}


int64_t Audio::getAVCallbackTime() {
    return av_gettime_relative();
}

double Audio::getClock() {

    if (this->playState->pause) {
        return pts;
    } else {
        return drift + av_gettime_relative() / AV_TIME_BASE;
    }


}
