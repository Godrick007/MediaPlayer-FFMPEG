//
// Created by Godrick Crown on 2020/11/4.
//


#include "SLProcessor.h"

SLProcessor::SLProcessor(int sampleRate) {
    this->sampleRate = sampleRate;
    this->pSampleBuffer = static_cast<SAMPLETYPE *>(malloc(sampleRate * 2 * 2));
    this->pSoundTouch = new SoundTouch();
    this->pSoundTouch->setSampleRate(sampleRate);
    this->pSoundTouch->setChannels(2);
    this->pSoundTouch->setPitch(this->pitch);
    this->pSoundTouch->setTempo(this->speed);
}

SLProcessor::~SLProcessor() {

}

void SLProcessor::initialize() {

    SLresult result;

    //engine
    result = slCreateEngine(&this->engineObject, 0, nullptr, 0, nullptr, nullptr);
    if (LOG_DEBUG) {
        LOGE("MediaPlayer", "SL Engine create is %d", result == SL_RESULT_SUCCESS);
    }
    (void) result;

    result = (*this->engineObject)->Realize(this->engineObject, SL_BOOLEAN_FALSE);

    result = (*this->engineObject)->GetInterface(this->engineObject, SL_IID_ENGINE,
                                                 &this->engineEngine);


    //mix
    const SLInterfaceID mixIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mixReq[1] = {SL_BOOLEAN_FALSE};

    result = (*this->engineEngine)->CreateOutputMix(this->engineEngine, &this->outputMixObject, 1,
                                                    mixIds,
                                                    mixReq);

    result = (*this->outputMixObject)->Realize(this->outputMixObject, SL_BOOLEAN_FALSE);

    result = (*this->outputMixObject)->GetInterface(this->outputMixObject,
                                                    SL_IID_ENVIRONMENTALREVERB,
                                                    &this->outputMixEnvironmentReverb);

    result = (*this->outputMixEnvironmentReverb)->SetEnvironmentalReverbProperties(
            this->outputMixEnvironmentReverb, &this->reverbSettings);


    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, this->outputMixObject};


    //data source

    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDBUFFERQUEUE, 2};

    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            static_cast<SLuint32>(getCurrentSampleRateForOpenSL(this->sampleRate)),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataSink audioSink = {&outputMix, nullptr};


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

    result = (*this->pcmPlayerObject)->Realize(this->pcmPlayerObject, SL_BOOLEAN_FALSE);


    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_PLAY,
                                                    &this->pcmPlayerPlayer);

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_VOLUME,
                                                    &this->pcmPlayerVolume);

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_MUTESOLO,
                                                    &this->pcmPlayerMute);

    result = (*this->pcmPlayerObject)->GetInterface(this->pcmPlayerObject, SL_IID_BUFFERQUEUE,
                                                    &this->pcmBufferQueue);



}


int SLProcessor::getCurrentSampleRateForOpenSL(int sampleRate) {
    int rate = 0;

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
