//
// Created by Godrick Crown on 2020/11/4.
//

#ifndef MEDIAPLAYER_FFMPEG_SLPROCESSOR_H
#define MEDIAPLAYER_FFMPEG_SLPROCESSOR_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../soundtouch/SoundTouch.h"
#include "../Callback2Java.h"
#include "Audio.h"


using namespace soundtouch;

class Audio;

class SLProcessor {

public:

    Audio *audio = nullptr;

    //engine
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine = nullptr;

    //mixer
    SLObjectItf outputMixObject = nullptr;
    SLEnvironmentalReverbItf outputMixEnvironmentReverb = nullptr;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm

    SLObjectItf pcmPlayerObject = nullptr;
    SLPlayItf pcmPlayerPlayer = nullptr;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = nullptr;

    SLVolumeItf pcmPlayerVolume = nullptr;
    SLMuteSoloItf pcmPlayerMute = nullptr;

//    SAMPLETYPE *pSoundTouchBuffer = nullptr;
    void *pPCMBuffer = nullptr;

    int sampleRate = 0;

    int channel = 0;


    long duration = 0;


public:

    SLProcessor(int sampleRate, Audio *audio);

    ~SLProcessor();

    void initialize();

    SLuint32 getCurrentSampleRateForOpenSL(int sampleRate);

   //void pcmBufferCallback(SLAndroidSimpleBufferQueueItf queueItf, void *context);

    int getPCMDB(char *pcmCate, size_t pcmSize);

    void pause();

    void stop();

    void resume();

    void release();

    void setVolume(int percent);
};


#endif //MEDIAPLAYER_FFMPEG_SLPROCESSOR_H
