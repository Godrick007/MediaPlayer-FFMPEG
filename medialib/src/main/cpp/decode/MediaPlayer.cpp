//
// Created by Godrick Crown on 2020/10/31.
//

#include "MediaPlayer.h"

int AVContextInterruptCallback(void *context) {
    MediaPlayer *instance = static_cast<MediaPlayer *>(context);
    if (instance->playState->exit) {
        return AVERROR_EOF;
    }
    return 0;
}


MediaPlayer::MediaPlayer(PlayState *playState, Callback2Java *cb2j, const char *url,
                         bool isLiving) {
    this->playState = playState;
    this->cb2j = cb2j;
    this->url = url;
    this->isLiving = isLiving;
    pthread_mutex_init(&mutexInit, nullptr);
    pthread_mutex_init(&mutexSeek, nullptr);
    prepare();
}

MediaPlayer::~MediaPlayer() {
    release();
    pthread_mutex_destroy(&mutexInit);
    pthread_mutex_destroy(&mutexSeek);
}

void *initThreadCallback(void *data) {
    MediaPlayer *instance = static_cast<MediaPlayer *>(data);
    instance->initialized();
    return 0;
}

void *playThreadCallback(void *data) {
    MediaPlayer *instance = static_cast<MediaPlayer *>(data);
    instance->startPlay();
    return 0;
}


void MediaPlayer::prepare() {
    pthread_create(&this->threadInit, nullptr, initThreadCallback, this);
}


void MediaPlayer::setAudio(Audio *audio) {
    this->audio = audio;
}

void MediaPlayer::play() {
    pthread_create(&this->threadPlay, nullptr, playThreadCallback, this);
}

void MediaPlayer::pause() {
    if (playState != nullptr) {
        playState->pause = true;
    }
    if (audio != nullptr) {
        audio->pause();
    }
}

void MediaPlayer::resume() {
    if (playState != nullptr) {
        playState->pause = false;
    }
    if (audio != nullptr) {
        audio->resume();
    }
}

void MediaPlayer::stop() {
//    release();
}

void MediaPlayer::release() {
    playState->exit = true;

    pthread_join(threadPlay, nullptr);

    pthread_mutex_lock(&mutexInit);

    int sleepCount = 0;

    while (!exit) {

        if (sleepCount > 1000) {
            exit = true;
        }
        sleepCount++;
        av_usleep(PlayState::THRESHOLD_SLEEP_10);
    }

    if (audio != nullptr) {
        delete audio;
        audio = nullptr;
    }

//    if (video != nullptr) {
//        delete video;
//        video = nullptr;
//    }

    if (pAVFormatContext != nullptr) {
        avformat_close_input(&pAVFormatContext);
        avformat_free_context(pAVFormatContext);
        pAVFormatContext = nullptr;
    }

    if (playState != nullptr) {
        playState = nullptr;
    }

    if (cb2j != nullptr) {
        cb2j = nullptr;
    }

    pthread_mutex_unlock(&mutexInit);
}

void MediaPlayer::seek(int64_t second) {

    if (duration < 0) {
        return;
    }

    if (second < 0 || second * 1000 > duration) {
        return;
    }

    playState->seek = true;
    pthread_mutex_lock(&mutexSeek);

    uint64_t rel = second * AV_TIME_BASE;
    avformat_seek_file(pAVFormatContext, -1, INT64_MIN, rel, INT64_MAX, 0);

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

void MediaPlayer::setVolume(int percent) {
    if (audio != nullptr) {
        audio->setVolume(percent);
    }
}

void MediaPlayer::setChannel(int channel) {

}

void MediaPlayer::setSpeed(float speed) {
    if (this->audio != nullptr) {
        audio->setSpeed(speed);
    }
}

void MediaPlayer::setPitch(float pitch) {
    if (this->audio != nullptr) {
        audio->setPitch(pitch);
    }
}

int MediaPlayer::getSampleRate() {
    return this->audio != nullptr ? this->audio->sampleRate : 0;
}

void MediaPlayer::startStopRecord(bool start) {

}

int MediaPlayer::getAVCodecContext(AVCodecParameters *param, AVCodecContext **codecContext) {

    AVCodec *pCodec = avcodec_find_decoder(param->codec_id);

    if (!pCodec) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't find decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_FIND_CODEC_ERROR,
                                         "can't find decoder");
        exit = true;
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
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }


    if (avcodec_parameters_to_context(*codecContext, param) < 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't fill decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_FILL_CODEC_ERROR,
                                         "can't fill decoder");
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }

    if (avcodec_open2(*codecContext, pCodec, nullptr) != 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Can't open decoder");
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, PlayState::MP_INIT_OPEN_CODEC_ERROR,
                                         "can't open decoder");
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return -1;
    }

    if (LOG_DEBUG) {
        LOGE("MediaPlayer", "mime is %s", (*codecContext)->codec_descriptor->name);
    }

    return 0;

}

void MediaPlayer::initialized() {

    pthread_mutex_lock(&this->mutexInit);

//    av_register_all();
    avformat_network_init();

    ret = avformat_open_input(&this->pAVFormatContext, this->url, nullptr, nullptr);

    if (ret != 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't open url : %s  errorCode is %d -- %s\n", this->url,
                 ret, av_err2str(ret));
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, ret, av_err2str(ret));
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return;
    }

    if (avformat_find_stream_info(pAVFormatContext, nullptr) < 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't find stream info : %s\n", this->url);
            //LOGE("MediaPlayer",);

            av_dump_format(pAVFormatContext, 0, this->url, 0);
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, 2, "can't find stream info");
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return;
    }

    this->pAVFormatContext->interrupt_callback.callback = AVContextInterruptCallback;
    this->pAVFormatContext->interrupt_callback.opaque = this;

    for (int i = 0; i < pAVFormatContext->nb_streams; i++) {

        if (pAVFormatContext->streams[i]->codecpar->codec_type ==
            AVMEDIA_TYPE_AUDIO) {//audio stream
            if (this->audio == nullptr) {
                this->audio = new Audio(this->playState,
                                        pAVFormatContext->streams[i]->codecpar->sample_rate,
                                        this->cb2j, this->isLiving);
                this->audio->streamIndex = i;
                this->audio->pAVCodecParameters = pAVFormatContext->streams[i]->codecpar;
                this->audio->duration = pAVFormatContext->duration / AV_TIME_BASE * 1000;
                this->audio->timeBase = {pAVFormatContext->streams[i]->time_base.num * 1000,
                                         pAVFormatContext->streams[i]->time_base.den};
                this->duration = audio->duration;
            }
        } else if (pAVFormatContext->streams[i]->codecpar->codec_type ==
                   AVMEDIA_TYPE_VIDEO) {//video stream
            if (this->video == nullptr) {
//                this->video = new Video(this->playState, this->cb2j);
//                this->video->streamIndex = i;
//                this->video->timeBase = pAVFormatContext->streams[i]->time_base;
//                this->video->pAVCodecParameters = pAVFormatContext->streams[i]->codecpar;
//
//                int num = pAVFormatContext->streams[i]->avg_frame_rate.num;
//                int den = pAVFormatContext->streams[i]->avg_frame_rate.den;
//                if (num != 0 && den != 0) {
//                    int fps = num / den;
//                    video->defaultDelayTime = 1.0f / fps;
//                }
            }
        }
    }

    int ret = 0;

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
    }

    if (playState && !playState->exit) {
        cb2j->cb2j_MediaPlayer_Prepared(WORK_THREAD);
    } else {
        exit = true;
    }


    pthread_mutex_unlock(&this->mutexInit);

}

void MediaPlayer::startPlay() {


//    if (this->audio == nullptr) {
//        if (LOG_DEBUG) {
//            LOGD("MediaPlayer", "audio is null");
//            return;
//        }
//    }
//
//    if (this->video == nullptr) {
//        if (LOG_DEBUG) {
//            LOGD("MediaPlayer", "video is null");
//            return;
//        }
//    }

    if (audio == nullptr /*&& video == nullptr*/) {
        if (LOG_DEBUG) {
            LOGD("MediaPlayer", "audio and video is null");
            return;
        }
    }

    if (video != nullptr && audio != nullptr) {
        this->video->audio = this->audio;
    }

    if (audio != nullptr) {
        this->audio->play();
    }

    if (video != nullptr) {
        this->video->play();
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

        ret = av_read_frame(this->pAVFormatContext, pkt);

        pthread_mutex_unlock(&mutexSeek);

        if (ret == 0) {
            if (pkt->stream_index == this->audio->streamIndex) {//audio pkt
                if (audio != nullptr) {
                    this->audio->queue->putAVPacket(pkt);
                }

            } /*else if (pkt->stream_index == this->video->streamIndex) {//video pkt
                if (video != nullptr) {
                    this->video->queue->putAVPacket(pkt);
                }
            }*/ else {
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

    exit = true;

    if (audio != nullptr) {
        audio->stop();
    }

    if (this->cb2j) {
        this->cb2j->cb2j_MediaPlayer_Complete(WORK_THREAD);
    }
}

void MediaPlayer::setRenderer(Renderer *renderer) {
    pthread_mutex_lock(&this->mutexInit);
    if (video) {
        video->setRenderer(renderer);
    }
    pthread_mutex_unlock(&this->mutexInit);
}






