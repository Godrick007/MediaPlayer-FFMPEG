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
    pthread_mutex_destroy(&this->mutexDecode);
}

inline void freeAVPacket(AVPacket **pkt) {
    av_packet_free(pkt);
}

inline void freeAVFrame(AVFrame **frame) {
    av_frame_free(frame);
}


void *threadDecode(void *context) {

//    修改到target宽高

    AVFrame *frame;

    auto *instance = static_cast<Video *>(context);

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

        pthread_mutex_lock(&instance->mutexDecode);


        if (instance->HWSupport) {


            if (instance->callbackHWCodec != nullptr) {

//                if(!instance->frameVertexFixed){
//                    instance->frameVertexFixed = true;
//
//                }
                if (av_bsf_send_packet(instance->pAVBSFContext, pkt) != 0) {
                    freeAVPacket(&pkt);
                    pthread_mutex_unlock(&instance->mutexDecode);
                    continue;
                }

                while (av_bsf_receive_packet(instance->pAVBSFContext, pkt) == 0) {

                    int diff = instance->getFrameDiffTime(nullptr, pkt);
                    av_usleep(instance->getDelayTime(diff) * AV_TIME_BASE);

                    instance->callbackHWCodec(pkt->data, pkt->size);
                }


            }

        } else {


            if (avcodec_send_packet(instance->pAVCodecContext, pkt) != 0) {
                freeAVPacket(&pkt);
                pthread_mutex_unlock(&instance->mutexDecode);
                continue;
            }

            frame = av_frame_alloc();

            if (avcodec_receive_frame(instance->pAVCodecContext, frame) != 0) {
                freeAVPacket(&pkt);
                freeAVFrame(&frame);
                pthread_mutex_unlock(&instance->mutexDecode);
                continue;
            }

            if (frame->format == AV_PIX_FMT_YUV420P) {

                int diff = instance->getFrameDiffTime(frame, nullptr);
                av_usleep(instance->getDelayTime(diff) * AV_TIME_BASE);

                if (instance->callbackYUVData != nullptr) {

                    instance->callbackYUVData(
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            frame->data[0],
                            frame->data[1],
                            frame->data[2]
                    );
                }


//            double delay = instance->fixTimeStamp(frame->pts);
//            if (delay > 0) {
//                av_usleep(delay * AV_TIME_BASE);
//            }

//            av_usleep(instance->defaultDelayTime * AV_TIME_BASE);

//opengles render

//            LOGE("clock ", "get clock is %lf", instance->audio->getClock());

//            instance->pts = frame->pts * av_q2d(instance->timeBase);
//            instance->last_updated = av_gettime_relative() / AV_TIME_BASE;




//            if (instance->renderer) {
//                instance->renderer->setYUVData(
//                        instance->pAVCodecContext->coded_width,
//                        instance->pAVCodecContext->coded_height,
//                        frame->data[0],
//                        frame->data[1],
//                        frame->data[2]
//                );
//                instance->cb2j->cb2j_MediaPlayer_Request_Render(WORK_THREAD);
//            }




            } else {


                AVFrame *dstFrame = av_frame_alloc();
                int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                          instance->pAVCodecContext->coded_width,
                                                          instance->pAVCodecContext->coded_height,
                                                          1);

                auto *buffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof(uint8_t)));


                av_image_fill_arrays(dstFrame->data,
                                     dstFrame->linesize,
                                     buffer,
                                     AV_PIX_FMT_YUV420P,
                                     instance->pAVCodecContext->coded_width,
                                     instance->pAVCodecContext->coded_height,
                                     1);


                if (instance->pSwsContext == nullptr) {
                    instance->pSwsContext = sws_getContext(
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            instance->pAVCodecContext->pix_fmt,
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                            nullptr, nullptr, nullptr
                    );
                } else {
                    instance->pSwsContext = sws_getCachedContext(
                            instance->pSwsContext,
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            instance->pAVCodecContext->pix_fmt,
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                            nullptr, nullptr, nullptr
                    );
                }


                if (instance->pSwsContext == nullptr) {
                    av_frame_free(&dstFrame);
                    av_free(buffer);
                    av_frame_free(&frame);
                    av_packet_free(&pkt);
                    pthread_mutex_unlock(&instance->mutexDecode);
                    continue;
                }

                sws_scale(instance->pSwsContext,
                          frame->data,
                          frame->linesize,
                          0,
                          frame->height,
                          dstFrame->data,
                          dstFrame->linesize
                );

                int diff = instance->getFrameDiffTime(dstFrame, nullptr);

                av_usleep(instance->getDelayTime(diff) * AV_TIME_BASE);

//            double diff = instance->fixTimeStamp(dstFrame->pts);
//
//            av_usleep(diff * AV_TIME_BASE);

                //render

                if (instance->renderer) {
                    instance->renderer->setYUVData(
                            instance->pAVCodecContext->coded_width,
                            instance->pAVCodecContext->coded_height,
                            dstFrame->data[0],
                            dstFrame->data[1],
                            dstFrame->data[2]
                    );
                    instance->cb2j->cb2j_MediaPlayer_Request_Render(WORK_THREAD);
                }


                av_frame_free(&frame);
                av_free(buffer);
            }

        }


        freeAVFrame(&frame);
        freeAVPacket(&pkt);
        pthread_mutex_unlock(&instance->mutexDecode);

    }
    return nullptr;
}

void Video::play() {


//    fp_yuv = fopen(output, "wb+");
//
//    LOGD("MediaPlayer", "fopen error with %d", errno);


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

    if (LOG_DEBUG) {
//        LOGD("MediaPlayer", "delay time is %e", delayTime);
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
    if (LOG_DEBUG) {
//        LOGD("MediaPlayer", "diff time is %e", diff);
    }
    return diff;
}

void Video::setRenderer(OldRenderer *renderer) {
    this->renderer = renderer;
    if (renderer != nullptr)
        renderer->setYUVSize(this->pAVCodecContext->coded_width,
                             this->pAVCodecContext->coded_height);
}

void Video::release() {

//    fclose(fp_yuv);

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
        pSwsContext = nullptr;
    }

    if (this->pAVCodecContext != nullptr) {
        pthread_mutex_lock(&this->mutexDecode);
        avcodec_close(pAVCodecContext);
        avcodec_free_context(&this->pAVCodecContext);
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

double Video::fixTimeStamp(double current_pts) {

    double diff = current_pts * av_q2d(this->timeBase) - audio->clock;
    if (diff <= 0) { //video slow

    } else if (diff >= 0) { //video fast
//        double delay = FFMIN(defaultDelayTime, diff);
//        return delay;
//        if (LOG_DEBUG) {
//            LOGE("MediaPlayer", "a pts = %lf, v pts = %lf, diff = %lf", audio->clock,
//                 current_pts * av_q2d(this->timeBase), diff);
//        }

        return diff;
    }
//    if (LOG_DEBUG) {
//        LOGD("MediaPlayer", "diff time is %e", diff);
//    }



    return 0;
}


void Video::setCallbackYUV(void (*pCallback)(int, int, void *, void *, void *)) {
    callbackYUVData = pCallback;
}

void Video::setCallbackHW(void (*pCallback)(void *, int)) {
    callbackHWCodec = pCallback;
}

void Video::setHWSupport(bool s) {
    HWSupport = s;
}






