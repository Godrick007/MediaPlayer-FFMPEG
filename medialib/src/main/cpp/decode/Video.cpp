//
// Created by Godrick Crown on 2020/11/2.
//

#include "Video.h"

Video::Video(PlayState *playState, Callback2Java *cb2j) {
    this->playState = playState;
    this->cb2j = cb2j;
    this->queue = new Queue(playState);
    pthread_mutex_init(&this->mutexDecode, nullptr);
}

Video::~Video() {
    release();
    pthread_mutex_destroy(&this->mutexDecode);
}

inline void freeAVPacket(AVPacket **pkt) {
    av_packet_free(pkt);
    av_free(*pkt);
    *pkt = nullptr;
}

inline void freeAVFrame(AVFrame **frame) {
    av_frame_free(frame);
    av_free(*frame);
    *frame = nullptr;
}


void *threadDecode(void *context) {

    Video *instance = static_cast<Video *>(context);

    while (instance->playState != nullptr && !instance->playState->exit) {

        if (instance->playState->seek) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;
        }

        if (instance->playState->pause) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;
        }

        if (instance->queue->getQueueSize() == 0) {

            if (!instance->playState->load) {
                instance->playState->load = true;
                instance->cb2j->cb2j_MediaPlayer_Loading(WORK_THREAD, true);
            }
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;

        } else {

            if (instance->playState->load) {
                instance->playState->load = false;
                instance->cb2j->cb2j_MediaPlayer_Loading(WORK_THREAD, false);
            }

        }

        AVPacket *pkt = av_packet_alloc();

        if (instance->queue->getAVPacket(pkt) != 0) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            freeAVPacket(&pkt);
            continue;
        }


        //media codec -- hardware


        //ffmpeg  -- software



        pthread_mutex_lock(&instance->mutexDecode);

        if (avcodec_send_packet(instance->pAVCodecContext, pkt) != 0) {
            freeAVPacket(&pkt);
            pthread_mutex_unlock(&instance->mutexDecode);
            continue;
        }

        AVFrame *frame = av_frame_alloc();

        if (avcodec_receive_frame(instance->pAVCodecContext, frame) != 0) {
            freeAVPacket(&pkt);
            freeAVFrame(&frame);
            pthread_mutex_unlock(&instance->mutexDecode);
            continue;
        }

        if (frame->format == AV_PIX_FMT_YUV420P) {

            int diff = instance->getFrameDiffTime(frame, nullptr);

            av_usleep(instance->getDelayTime(diff) * AV_TIME_BASE);
            //opengles render

            if (instance->renderer) {
                instance->renderer->setYUVData(
                        instance->pAVCodecContext->width,
                        instance->pAVCodecContext->height,
                        frame->data[0],
                        frame->data[1],
                        frame->data[2]
                );
            }


        } else {


            AVFrame *frameYUV420P = av_frame_alloc();
//            int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
//                                                      instance->pAVCodecContext->width,
//                                                      instance->pAVCodecContext->height,
//                                                      1);
//
//            uint8_t *buffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof(uint8_t)));


//            av_image_fill_arrays(frameYUV420P->data,
//                                 frameYUV420P->linesize,
//                                 frame->data,
//                                 AV_PIX_FMT_YUV420P,
//                                 instance->pAVCodecContext->width,
//                                 instance->pAVCodecContext->height,
//                                 1);


            if (instance->pSwsContext == nullptr) {
                instance->pSwsContext = sws_getContext(
                        instance->pAVCodecContext->width,
                        instance->pAVCodecContext->height,
                        instance->pAVCodecContext->pix_fmt,
                        instance->pAVCodecContext->width,
                        instance->pAVCodecContext->height,
                        AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                        nullptr, nullptr, nullptr
                );
            }


            if (instance->pSwsContext == nullptr) {
                av_frame_free(&frameYUV420P);
                av_free(frameYUV420P);
//                av_free(buffer);
                pthread_mutex_unlock(&instance->mutexDecode);
                continue;
            }

            sws_scale(instance->pSwsContext,
                      frame->data,
                      frame->linesize,
                      0,
                      frame->height,
                      frameYUV420P->data,
                      frameYUV420P->linesize
            );

            av_usleep(instance->getDelayTime(frame->best_effort_timestamp) * AV_TIME_BASE);

            //render

            if (instance->renderer) {
                instance->renderer->setYUVData(
                        instance->pAVCodecContext->width,
                        instance->pAVCodecContext->height,
                        frameYUV420P->data[0],
                        frameYUV420P->data[1],
                        frameYUV420P->data[2]
                );
            }


            av_frame_free(&frameYUV420P);
            av_free(frameYUV420P);
            //av_free(buffer);
//


        }

        freeAVFrame(&frame);
        freeAVPacket(&pkt);
        pthread_mutex_unlock(&instance->mutexDecode);

    }

}

void Video::play() {


    if (this->playState != nullptr && !this->playState->exit) {
        pthread_create(&threadPlay, nullptr, threadDecode, this);
    }
}


double Video::getDelayTime(double diff) {

    if (diff > 0.003) {
        delayTime = delayTime * 2 / 3;

        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff < -0.003) {
        delayTime = delayTime * 3 / 2;

        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }

    } else if (diff == 0.003) {

    }

    if (diff >= 0.5) {
        delayTime = 0;
    } else if (diff <= -0.5) {
        delayTime = defaultDelayTime * 2;
    }

    if (fabs(diff) >= 10) {
        delayTime = defaultDelayTime;
    }


    return delayTime;
}

double Video::getFrameDiffTime(AVFrame *frame, AVPacket *pkt) {


    double pts = 0;

    if (frame != nullptr) {
        pts = frame->best_effort_timestamp;
    } else if (pkt != nullptr) {
        pts = pkt->pts;
    }

    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }

    pts *= av_q2d(timeBase);

    if (pts > 0) {
        clock = pts;
    }

    double diff = audio->clock - clock;

    return diff;
}

void Video::setRenderer(Renderer *renderer) {
    this->renderer = renderer;
    renderer->setYUVSize(this->pAVCodecContext->width, this->pAVCodecContext->height);
}

void Video::release() {
    if (queue != nullptr) {
        queue->noticeAVPackQueue();
    }

    pthread_join(this->threadPlay, nullptr);

    if (queue != nullptr) {
        delete queue;
        queue = nullptr;
    }

    if (pSwsContext != nullptr) {
        sws_freeContext(pSwsContext);
        av_free(pSwsContext);
        pSwsContext = nullptr;
    }

    if (this->pAVCodecContext != nullptr) {
        pthread_mutex_lock(&this->mutexDecode);
        avcodec_close(pAVCodecContext);
        avcodec_free_context(&this->pAVCodecContext);
        av_free(pAVCodecContext);
        pAVCodecContext = nullptr;
        pthread_mutex_unlock(&this->mutexDecode);
    }


    if (playState != nullptr) {
        playState = nullptr;
    }

    if (cb2j != nullptr) {
        cb2j = nullptr;
    }


}
