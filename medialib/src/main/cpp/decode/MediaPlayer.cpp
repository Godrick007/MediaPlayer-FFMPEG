//
// Created by Godrick Crown on 2020/10/31.
//

#include "MediaPlayer.h"

MediaPlayer::MediaPlayer(PlayState *playState, Callback2Java *cb2j, const char *url) {
    this->playState = playState;
    this->cb2j = cb2j;
    this->url = url;
    pthread_mutex_init(&mutexInit, nullptr);
    pthread_mutex_init(&mutexSeek, nullptr);

}

MediaPlayer::~MediaPlayer() {

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

}

void MediaPlayer::resume() {

}

void MediaPlayer::stop() {

}

void MediaPlayer::release() {

}

void MediaPlayer::seek(int64_t second) {

}

void MediaPlayer::setVolume(int percent) {

}

void MediaPlayer::setChannel(int channel) {

}

void MediaPlayer::setSpeed(float speed) {

}

void MediaPlayer::setPitch(float pitch) {

}

int MediaPlayer::getSampleRate() {
    return 0;
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

    return 0;

}

void MediaPlayer::initialized() {

    pthread_mutex_lock(&this->mutexInit);

    //av_register_all();
    avformat_network_init();

    if (avformat_open_input(&this->pAVFormatContext, this->url, nullptr, nullptr) != 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't open url : %s\n", this->url);
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, 1, "Url error");
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return;
    }

    if (avformat_find_stream_info(pAVFormatContext, nullptr) < 0) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "FFMPEG can't find stream info : %s\n", this->url);
        }
        cb2j->cb2j_MediaPlayer_InitError(WORK_THREAD, 2, "can't find stream info");
        exit = true;
        pthread_mutex_unlock(&this->mutexInit);
        return;
    }

    for (int i = 0; i < pAVFormatContext->nb_streams; i++) {

        if (pAVFormatContext->streams[i]->codecpar->codec_type ==
            AVMEDIA_TYPE_AUDIO) {//audio stream
            if (this->audio == nullptr) {
                this->audio = new Audio(this->playState,
                                        pAVFormatContext->streams[i]->codecpar->sample_rate,
                                        this->cb2j);
                this->audio->streamIndex = i;
                this->audio->pAVCodecParameters = pAVFormatContext->streams[i]->codecpar;
                this->audio->duration = pAVFormatContext->duration / AV_TIME_BASE;
                this->audio->timeBase = pAVFormatContext->streams[i]->time_base;
                this->duration = audio->duration;
            }
        } else if (pAVFormatContext->streams[i]->codecpar->codec_type ==
                   AVMEDIA_TYPE_VIDEO) {//video stream
            if (this->video == nullptr) {
                this->video = new Video(this->playState, this->cb2j);
                this->video->streamIndex = i;
                this->video->timeBase = pAVFormatContext->streams[i]->time_base;
                this->video->pAVCodecParameters = pAVFormatContext->streams[i]->codecpar;

                int num = pAVFormatContext->streams[i]->avg_frame_rate.num;
                int den = pAVFormatContext->streams[i]->avg_frame_rate.den;
                if (num != 0 && den != 0) {
                    int fps = num / den;
                    video->defaultDelayTime = 1.0f / fps;
                }
            }
        }
    }

    int ret = 0;

    if (this->audio) {
        ret = getAVCodecContext(this->audio->pAVCodecParameters, &this->audio->pAVCodecContext);
        if (ret < 0) {
            return;
        }
    }

    if (this->video) {
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
    if (!this->audio) {
        if (LOG_DEBUG) {
            LOGD("MediaPlayer", "audio is null");
            return;
        }
    }

    if (this->video) {
        if (LOG_DEBUG) {
            LOGD("MediaPlayer", "audio is null");
            return;
        }
    }

    this->video->audio = this->audio;

    this->audio->play();

    this->video->play();

    while (this->playState && !this->playState->exit) {

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

        int ret = av_read_frame(this->pAVFormatContext, pkt);

        pthread_mutex_unlock(&mutexSeek);

        if (ret == 0) {
            if (pkt->stream_index == this->audio->streamIndex) {//audio pkt
                this->audio->queue->putAVPacket(pkt);
            } else if (pkt->stream_index == this->video->streamIndex) {//video pkt
                this->video->queue->putAVPacket(pkt);
            } else {
                av_packet_free(&pkt);
                av_free(pkt);
                pkt = nullptr;
            }
        } else {
            av_packet_free(&pkt);
            av_free(pkt);
            pkt = nullptr;

            while (this->playState && !this->playState->exit) {
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
    if (this->cb2j) {
        this->cb2j->cb2j_MediaPlayer_Complete(WORK_THREAD);
    }
}
