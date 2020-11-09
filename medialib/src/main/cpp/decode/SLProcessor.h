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

//extern "C"
//{
//#include <libavutil/mem.h>
//};

using namespace soundtouch;

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
    SoundTouch *pSoundTouch;
    SLObjectItf pcmPlayerObject = nullptr;
    SLPlayItf pcmPlayerPlayer = nullptr;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = nullptr;

    SLVolumeItf pcmPlayerVolume = nullptr;
    SLMuteSoloItf pcmPlayerMute = nullptr;

    SAMPLETYPE *pSampleBuffer = nullptr;


    float speed = 1.0f;
    float pitch = 1.0f;

    int sampleRate = 0;

    int channel = 0;

    long clock = 0;
    long duration = 0;


public:

    SLProcessor(int sampleRate);

    ~SLProcessor();

    void initialize();

    SLuint32 getCurrentSampleRateForOpenSL(int sampleRate);

//    void pcmBufferCallback(SLAndroidSimpleBufferQueueItf queueItf, void *context);

    int getSoundTouchData();

    int getPCMDB(char *pcmCate, size_t pcmSize);

};


#endif //MEDIAPLAYER_FFMPEG_SLPROCESSOR_H
