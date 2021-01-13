//
// Created by Godrick Crown on 2020/11/4.
//


#include "SLProcessor.h"

void pcmBufferCallback(SLAndroidSimpleBufferQueueItf queueItf, void *context) {

    SLProcessor *instance = static_cast<SLProcessor *>(context);

    if (instance != nullptr) {

        int64_t audio_callback_time = instance->audio->getAVCallbackTime();
        int len, bufferSize = instance->audio->resampleAudio(&instance->pPCMBuffer);
//        int bufferSize = instance->audio->getSoundTouchData(
//                reinterpret_cast<void **>(&instance->pPCMBuffer));
        if (bufferSize > 0) {
            instance->audio->clock += bufferSize / ((double) instance->audio->sampleRate * 2 * 2);
            if (instance->audio->clock - instance->audio->lastTime >= 0.1) {
                instance->audio->lastTime = instance->audio->clock;
                instance->audio->cb2j->cb2j_MediaPlayer_Progress(WORK_THREAD,
                                                                 instance->audio->clock,
                                                                 instance->audio->duration);
            }
//            instance->audio->cb2j->cb2j_MediaPlayer_DBValue(WORK_THREAD,
//                                                            instance->getPCMDB(
//                                                                    /*reinterpret_cast<char *>(instance->pSoundTouchBuffer),*/
//                                                                    static_cast<char *>(instance->pPCMBuffer),
//                                                                    bufferSize));
            /*  (*instance->pcmBufferQueue)->Enqueue(instance->pcmBufferQueue,
                                                   instance->pSoundTouchBuffer,
                                                   bufferSize * 2 * 2);*/
            (*instance->pcmBufferQueue)->Enqueue(instance->pcmBufferQueue, instance->pPCMBuffer,
                                                 bufferSize);



//            instance->audio->hw_buffer_size = queueItf

            if (instance->audio->buffer_index >= instance->audio->buffer_size) {
                instance->audio->buffer_index = 0;
            }

            len = instance->audio->buffer_size - instance->audio->buffer_index;

            instance->audio->buffer_index += len;

            instance->audio->buffer_size = bufferSize;

            instance->audio->write_buffer_size =
                    instance->audio->buffer_size - instance->audio->buffer_index;


            instance->audio->setClock(
                    instance->audio->audio_clock - (double) (2 * instance->audio->hw_buffer_size +
                                                             instance->audio->write_buffer_size) /
                                                   instance->audio->bytes_per_sec,
                    audio_callback_time / AV_TIME_BASE,
                    instance->audio->serial

            );
        }

    }


}

SLProcessor::SLProcessor(int sampleRate, Audio *audio) {
    this->sampleRate = sampleRate;
    this->audio = audio;
//    this->pSoundTouchBuffer = static_cast<SAMPLETYPE *>(malloc(sampleRate * 2 * 2));
    this->pPCMBuffer = malloc(sampleRate * 2 * 2 * 2);

}

SLProcessor::~SLProcessor() {
    stop();
    release();
}

void SLProcessor::initialize() {

    SLresult result;

    //engine
    result = slCreateEngine(&this->engineObject, 0, nullptr, 0, nullptr, nullptr);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL slCreateEngine %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->engineObject)->Realize(this->engineObject, SL_BOOLEAN_FALSE);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL Engine Realize is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->engineObject)->GetInterface(this->engineObject, SL_IID_ENGINE,
                                                 &this->engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL Engine GetInterface is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    //mix
    const SLInterfaceID mixIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mixReq[1] = {SL_BOOLEAN_FALSE};

    result = (*this->engineEngine)->CreateOutputMix(this->engineEngine, &this->outputMixObject, 1,
                                                    mixIds,
                                                    mixReq);
    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL Engine CreateOutputMix is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->outputMixObject)->Realize(this->outputMixObject, SL_BOOLEAN_FALSE);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL outputMixObject Realize is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->outputMixObject)->GetInterface(this->outputMixObject,
                                                    SL_IID_ENVIRONMENTALREVERB,
                                                    &this->outputMixEnvironmentReverb);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL outputMixObject GetInterface is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->outputMixEnvironmentReverb)->SetEnvironmentalReverbProperties(
            this->outputMixEnvironmentReverb, &this->reverbSettings);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer",
                 "SL outputMixEnvironmentReverb SetEnvironmentalReverbProperties is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;


    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, this->outputMixObject};
    SLDataSink audioSink = {&outputMix, nullptr};

    //data source

    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};


    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            getCurrentSampleRateForOpenSL(this->sampleRate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};


    //player
    const SLInterfaceID playerIds[4] = {
            SL_IID_BUFFERQUEUE,
            SL_IID_VOLUME,
            SL_IID_MUTESOLO,
            SL_IID_PLAYBACKRATE
    };
    const SLboolean playerReq[4] = {
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE
    };

    result = (*this->engineEngine)->CreateAudioPlayer(
            this->engineEngine,
            &this->pcmPlayerObject,
            &slDataSource,
            &audioSink,
            4,
            playerIds,
            playerReq
    );

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL Engine CreateAudioPlayer is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->pcmPlayerObject)->Realize(this->pcmPlayerObject, SL_BOOLEAN_FALSE);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL Engine Realize is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;


    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_PLAY,
                                                    &this->pcmPlayerPlayer);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmPlayerObject GetInterface  pcmPlayerPlayer is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_VOLUME,
                                                    &this->pcmPlayerVolume);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmPlayerObject GetInterface  pcmPlayerVolume is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_MUTESOLO,
                                                    &this->pcmPlayerMute);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmPlayerObject GetInterface  pcmPlayerMute is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_BUFFERQUEUE,
                                                    &this->pcmBufferQueue);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmPlayerObject GetInterface  pcmBufferQueue is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    result = (*this->pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallback, this);

    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmBufferQueue RegisterCallback is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;


    result = (*this->pcmPlayerPlayer)->SetPlayState(pcmPlayerPlayer, SL_PLAYSTATE_PLAYING);


    if (result != SL_RESULT_SUCCESS) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "SL pcmPlayerPlayer SetPlayState is %d", result);
        }
        this->audio->cb2j->cb2j_MediaPlayer_SL_InitError(WORK_THREAD, result, "");
        return;
    }
    (void) result;

    pcmBufferCallback(pcmBufferQueue, this);

}


SLuint32 SLProcessor::getCurrentSampleRateForOpenSL(int sampleRate) {
    SLuint32 rate = 0;

    switch (sampleRate) {

        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
            break;


    }

    return rate;
}


int SLProcessor::getPCMDB(char *pcmCate, size_t pcmSize) {

    int db = 0;

    short int prevalue = 0;

    double sum = 0;

    for (int i = 0; i < pcmSize; i++) {

        memcpy(&prevalue, pcmCate + i, 2);
        sum += abs(prevalue);
    }

    sum /= (pcmSize / 2);

    if (sum > 0) {
        db = 20 * log10(sum);
    }

    return db;
}

void SLProcessor::pause() {
    if (pcmPlayerPlayer != nullptr) {
        (*pcmPlayerPlayer)->SetPlayState(pcmPlayerPlayer, SL_PLAYSTATE_PAUSED);
    }
}

void SLProcessor::stop() {
    if (pcmPlayerPlayer != nullptr) {
        (*pcmPlayerPlayer)->SetPlayState(pcmPlayerPlayer, SL_PLAYSTATE_STOPPED);
    }
}

void SLProcessor::resume() {
    if (pcmPlayerPlayer != nullptr) {
        (*pcmPlayerPlayer)->SetPlayState(pcmPlayerPlayer, SL_PLAYSTATE_PLAYING);
    }
}

void SLProcessor::release() {

    if (pcmPlayerObject != nullptr) {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = nullptr;
        pcmPlayerPlayer = nullptr;
        pcmPlayerVolume = nullptr;
        pcmPlayerMute = nullptr;
        pcmBufferQueue = nullptr;
    }

    if (outputMixObject != nullptr) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
        outputMixEnvironmentReverb = nullptr;
    }

    if (engineObject != nullptr) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineEngine = nullptr;
    }

    if (pPCMBuffer != nullptr) {
        free(pPCMBuffer);
        pPCMBuffer = nullptr;
    }

    if (this->audio != nullptr) {
        audio = nullptr;
    }
}

void SLProcessor::setVolume(int percent) {

    if (pcmPlayerVolume != nullptr) {
        int div = 0;

        if (percent > 30) {
            div = 22;
        } else if (percent > 25) {
            div = 25;
        } else if (percent > 20) {
            div = 28;
        } else if (percent > 15) {
            div = 30;
        } else if (percent > 10) {
            div = 33;
        } else if (percent > 5) {
            div = 35;
        } else if (percent > 0) {
            div = 40;
        } else {
            div = 100;
        }

        (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) * -div);
    }

}


