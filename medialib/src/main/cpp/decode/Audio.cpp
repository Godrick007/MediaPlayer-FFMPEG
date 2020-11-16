//
// Created by Godrick Crown on 2020/11/2.
//

#include "Audio.h"

Audio::Audio(PlayState *playState, int sampleRate, Callback2Java *cb2j) {
    this->playState = playState;
    this->sampleRate = sampleRate;
    this->cb2j = cb2j;
    this->pSLProcessor = new SLProcessor(sampleRate, this);
    this->queue = new Queue(playState);
    this->buffer = static_cast<uint8_t *>(av_malloc(sampleRate * 2 * 2));
    this->outSampleBuffer = reinterpret_cast<void **>(static_cast<SAMPLETYPE **>(malloc(
            sampleRate * 2 * 2)));

    this->pSoundTouch = new SoundTouch();
    this->pSoundTouch->setSampleRate(sampleRate);
    this->pSoundTouch->setChannels(2);
    this->pSoundTouch->setPitch(this->pitch);
    this->pSoundTouch->setTempo(this->speed);

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

        //decode

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

            SwrContext *pSwrContext = nullptr;

            pSwrContext = swr_alloc_set_opts(
                    nullptr,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    this->pAVFrame->sample_rate,
                    this->pAVFrame->channel_layout,
                    (AVSampleFormat) this->pAVFrame->format,
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
                pthread_mutex_unlock(&this->mutexDecode);
                continue;
            }


            this->nb = swr_convert(
                    pSwrContext,
                    &this->buffer,
                    this->pAVFrame->nb_samples,
                    (const uint8_t **) (this->pAVFrame->data),
                    this->pAVFrame->nb_samples
            );

            int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

            this->dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            this->nowTime = this->pAVFrame->pts * av_q2d(this->timeBase);

            if (this->nowTime < this->clock) {
                this->nowTime = this->clock;
            }
            this->clock = this->nowTime;

            *pcmBuffer = reinterpret_cast<SAMPLETYPE *>(this->buffer);


            av_frame_free(&this->pAVFrame);
            av_free(this->pAVFrame);
            this->pAVFrame = nullptr;
            av_packet_free(&this->pAVPacket);
            av_free(this->pAVPacket);
            this->pAVPacket = nullptr;
            swr_free(&pSwrContext);
            pSwrContext = nullptr;
            pthread_mutex_unlock(&this->mutexDecode);
            break;

        }


    }


    return this->dataSize;
}

//int Audio::getSoundTouchData(SAMPLETYPE **sampleBuffer) {
//
//    while (this->playState != nullptr && !this->playState->exit) {
//
//        uint8_t *outBuffer = nullptr;
//        if (this->finish) {
//            finish = false;
//            dataSize = resampleAudio(reinterpret_cast<void **>(&outBuffer));
//
//            if (dataSize > 0) {
//
//                for (int i = 0; i < dataSize / 2 + 1; i++) {
//                    (*sampleBuffer)[i] = (outBuffer[i * 2] | outBuffer[i * 2 + 1] << 8);
//                }
//
//                this->pSoundTouch->putSamples(*sampleBuffer, nb);
//                this->num = this->pSoundTouch->receiveSamples(
//                        *sampleBuffer, dataSize / 4);
//
//            } else {
//                this->pSoundTouch->flush();
//            }
//        }
//
//        if (num == 0) {
//            finish = true;
//            continue;
//        } else {
//
//            if (outBuffer == nullptr) {
//                this->num = this->pSoundTouch->receiveSamples(
//                        *sampleBuffer, dataSize / 4);
//                if (num == 0) {
//                    finish = true;
//                    continue;
//                }
//            }
//            return num;
//        }
//
//
//    }
//    return 0;
//}

int Audio::getSoundTouchData(SAMPLETYPE **sampleBuffer) {

    while (this->playState != nullptr && !this->playState->exit) {

        uint8_t *outBuffer = nullptr;
        if (this->finish) {
            finish = false;
            dataSize = resampleAudio(reinterpret_cast<void **>(&outBuffer));
            *sampleBuffer = reinterpret_cast<SAMPLETYPE *>(outBuffer);
            return dataSize;
        }

        if (dataSize == 0) {
            finish = true;
            continue;
        } else {
            finish = true;
            continue;
        }
    }
    return 0;
}

