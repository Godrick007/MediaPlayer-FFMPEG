//
// Created by Godrick Crown on 2021/1/22.
//

#include "MediaController.h"

GLThread *MediaController::glThread;
YUVRenderer *MediaController::render;

int AVContextInterruptCallback(void *context) {
    MediaController *instance = static_cast<MediaController *>(context);
    if (instance->playState->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

void *threadCallbackInit(void *context) {
    MediaController *instance = static_cast<MediaController *>(context);
    instance->threadCallbackMediaInitialize();
    return nullptr;
}

void *threadCallbackDemuexer(void *context) {
    MediaController *instance = static_cast<MediaController *>(context);
    instance->threadCallbackMediaDemuxer();
    return nullptr;
}


MediaController::MediaController(PlayState *ps, Callback2Java *cj, Audio *a, Video *v) {
    this->playState = ps;
    this->cb2j = cj;
    this->audio = a;
    this->video = v;
    this->render = new YUVRenderer();
    this->glThread = nullptr;
    pthread_mutex_init(&mutexInit, nullptr);
    pthread_mutex_init(&mutexSeek, nullptr);

}

void MediaController::prepare(const char *url) {
    this->url = url;
    pthread_create(&threadInit, nullptr, threadCallbackInit, this);
}

void MediaController::threadCallbackMediaInitialize() {

    int ret = 0;

    pthread_mutex_lock(&mutexInit);

    avformat_network_init();

    ret = avformat_open_input(&pAVFormatContextInput, url, nullptr, nullptr);

    if (ret != 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't open url : %s  errorCode is %d -- %s\n", url,
                 ret, av_err2str(ret));
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, ret, av_err2str(ret));
        playState->exit = true;
        pthread_mutex_unlock(&mutexInit);
        return;
    }

    ret = avformat_find_stream_info(pAVFormatContextInput, nullptr);

    if (ret < 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't find stream info : %s\n", url);
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, 2, "can't find stream info");
        playState->exit = true;
        pthread_mutex_unlock(&mutexInit);
        return;
    }

    pAVFormatContextInput->interrupt_callback.callback = AVContextInterruptCallback;
    pAVFormatContextInput->interrupt_callback.opaque = this;

    for (int i = 0; i < pAVFormatContextInput->nb_streams; i++) {

        if (pAVFormatContextInput->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            initAudio(i);
        } else if (pAVFormatContextInput->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            initVideo(i);
        } else if (pAVFormatContextInput->streams[i]->codecpar->codec_type ==
                   AVMEDIA_TYPE_SUBTITLE) {
            initSubtitle(i);
        } else {

        }

    }

    if (this->audio != nullptr) {
        ret = getAVCodecContext(this->audio->pAVCodecParameters, &this->audio->pAVCodecContext);
        if (ret < 0) {
            return;
        }
    }

    if (this->video != nullptr) {
        ret = getAVCodecContext(this->video->pAVCodecParameters, &this->video->pAVCodecContext);

        if (ret < 0) {
            return;
        }
//        this->video->setRenderer(this->render);
        this->render->setYUVSize(video->pAVCodecContext->coded_width,
                                 video->pAVCodecContext->coded_height);
    }

    if (playState && !playState->exit) {
        cb2j->cb2j_MediaPlayer_Prepared(WORK_THREAD);
    } else {
        playState->exit = true;
    }


    pthread_mutex_unlock(&mutexInit);
}

void MediaController::threadCallbackMediaDemuxer() {

    int ret;


    if (audio != nullptr) {
        this->audio->play();
    }

    if (video != nullptr) {
        this->video->play();
    }

    if (audio != nullptr && video != nullptr) {
        video->audio = audio;
    }

    while (this->playState != nullptr && !this->playState->exit) {

        if (this->playState->seek) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;
        }

        if (this->audio->queue->getQueueSize() > PlayState::THRESHOLD_QUEUE_LIMIT) {
            av_usleep(PlayState::THRESHOLD_SLEEP_100);
            continue;
        }

        AVPacket *pkt = av_packet_alloc();

        pthread_mutex_lock(&mutexSeek);

        ret = av_read_frame(this->pAVFormatContextInput, pkt);

        pthread_mutex_unlock(&mutexSeek);

        if (ret == 0) {
            if (pkt->stream_index == this->audio->streamIndex) {//audio pkt
                if (audio != nullptr) {
                    this->audio->queue->putAVPacket(pkt);
                }

            } else if (pkt->stream_index == this->video->streamIndex) {//video pkt
                if (video != nullptr) {
                    this->video->queue->putAVPacket(pkt);
                }
            } else {
                av_packet_free(&pkt);
                av_free(pkt);
            }
        } else {
            av_packet_free(&pkt);
            av_free(pkt);

            while (this->playState != nullptr && !this->playState->exit) {
                if (audio->queue->getQueueSize() > 0) {
                    av_usleep(PlayState::THRESHOLD_SLEEP_500);
                    continue;
                } else {
                    if (!playState->seek) {
                        av_usleep(PlayState::THRESHOLD_SLEEP_500);
                        playState->exit = true;
                    }
                    break;
                }
            }
            break;
        }
    }

    this->playState->exit = true;

    if (audio != nullptr) {
        audio->stop();
    }

    if (this->cb2j) {
        this->cb2j->cb2j_MediaPlayer_Complete(WORK_THREAD);
    }


}

void MediaController::initAudio(int streamIndex) {
    if (audio == nullptr) {
        audio = new Audio(playState,
                          pAVFormatContextInput->streams[streamIndex]->codecpar->sample_rate,
                          cb2j, false);
    }
    audio->streamIndex = streamIndex;
    audio->pAVCodecParameters = pAVFormatContextInput->streams[streamIndex]->codecpar;
    audio->duration = pAVFormatContextInput->duration / AV_TIME_BASE;
    audio->timeBase = pAVFormatContextInput->streams[streamIndex]->time_base;
    duration = audio->duration;
}

void MediaController::initVideo(int streamIndex) {
    if (this->video == nullptr) {
        this->video = new Video(this->playState, this->cb2j);
    }
    this->video->streamIndex = streamIndex;
    this->video->timeBase = pAVFormatContextInput->streams[streamIndex]->time_base;
    this->video->pAVCodecParameters = pAVFormatContextInput->streams[streamIndex]->codecpar;
    this->video->setCallback(this->callbackYUVData);
    int num = pAVFormatContextInput->streams[streamIndex]->avg_frame_rate.num;
    int den = pAVFormatContextInput->streams[streamIndex]->avg_frame_rate.den;
    if (num != 0 && den != 0) {
        int fps = num / den;
        video->defaultDelayTime = 1.0f / fps;
    }

}

void MediaController::initSubtitle(int streamIndex) {

}

int MediaController::getAVCodecContext(AVCodecParameters *param, AVCodecContext **codecContext) {
    AVCodec *pCodec = avcodec_find_decoder(param->codec_id);

    if (!pCodec) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't find decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_FIND_CODEC_ERROR,
                                         "can't find decoder");
        playState->exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }

    *codecContext = avcodec_alloc_context3(pCodec);

    if (!*codecContext) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't alloc decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_ALLOC_CODEC_ERROR,
                                         "can't alloc decoder");
        playState->exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }


    if (avcodec_parameters_to_context(*codecContext, param) < 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't fill decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_FILL_CODEC_ERROR,
                                         "can't fill decoder");
        playState->exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }

    if (avcodec_open2(*codecContext, pCodec, nullptr) != 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't open decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_OPEN_CODEC_ERROR,
                                         "can't open decoder");
        playState->exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }

    if (LOG_DEBUG) {
        LOGE("MediaPlayer", "mime is %s", (*codecContext)->codec_descriptor->name);
    }

    return 0;
}

void MediaController::surfaceCreate(ANativeWindow *surface) {

    LOGI("c_tag", "MediaController::surfaceCreate");

    this->surface = surface;
    glThread = new GLThread(surface);
    glThread->setRenderer(render);
    glThread->start();
}

void MediaController::surfaceChange(int width, int height) {

    LOGI("c_tag", "MediaController::surfaceChange glThread is %d", glThread != nullptr);

    if (glThread != nullptr) {
        glThread->setChanged(width, height);
    }
}

void MediaController::surfaceDraw() {

}

void MediaController::setRenderMode(RenderMode mode) {

    if (mode != RENDERMODE_CONTINUOUSLY && mode != RENDERMODE_WHEN_DIRTY) {
        return;
    }
    renderMode = mode;
    if (glThread != nullptr) {
        glThread->setRenderMode(mode);
    }
}

void MediaController::callbackYUVData(int width, int height, void *y, void *u, void *v) {
    if (render != nullptr) {
        render->setYUVData(width, height, y, u, v);
    }
    if (glThread != nullptr) {
        glThread->requestRender();
    }
}

void MediaController::start() {
    pthread_create(&threadDemuxer, nullptr, threadCallbackDemuexer, this);
}

void MediaController::pause() {
    this->audio->pause();
}

void MediaController::resume() {
    this->audio->resume();
}

void MediaController::stop() {
    this->audio->stop();
}

void MediaController::seekTo(long second) {

    if (duration < 0) {
        return;
    }

    if (second < 0 || second * 1000 > duration) {
        return;
    }

    playState->seek = true;
    pthread_mutex_lock(&mutexSeek);

    uint64_t rel = second * AV_TIME_BASE;
    avformat_seek_file(pAVFormatContextInput, -1, INT64_MIN, rel, INT64_MAX, 0);

    if (audio != nullptr) {
        audio->queue->clearAVPacketQueue();
        audio->clock = 0;
        audio->lastTime = 0;
        pthread_mutex_lock(&audio->mutexDecode);
        avcodec_flush_buffers(audio->pAVCodecContext);
        pthread_mutex_unlock(&audio->mutexDecode);
    }

    if (video != nullptr) {
        video->queue->clearAVPacketQueue();
        video->clock = 0;
        pthread_mutex_lock(&video->mutexDecode);
        avcodec_flush_buffers(video->pAVCodecContext);
        pthread_mutex_unlock(&video->mutexDecode);
    }


    pthread_mutex_unlock(&mutexSeek);
    playState->seek = false;


}

void MediaController::release() {

}

MediaController::~MediaController() {

}







