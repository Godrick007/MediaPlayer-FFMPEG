//
// Created by Godrick Crown on 2021/1/22.
//

#ifndef MEDIAPLAYER_FFMPEG_MEDIACONTROLLER_H
#define MEDIAPLAYER_FFMPEG_MEDIACONTROLLER_H


#include "../decode/Audio.h"
#include "../decode/Video.h"
#include "../egl/GLThread.h"
#include "../egl/EGLHelper.h"
#include "../render/YUVRenderer.h"
#include "PlayState.h"
#include "../decode/HWDecoder.h"
#include <string>

#include <android/native_window.h>
#include <pthread.h>


extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
};

using namespace std;

enum RenderType {
    RENDER_WHEN_DIRTY,
    RENDER_CONTINUE
};

class MediaController {

public :

    long duration = 0;

    const char *url = nullptr;
    string mimeType;
    Audio *audio = nullptr;
    Video *video = nullptr;
    Callback2Java *cb2j = nullptr;
    PlayState *playState = nullptr;
    static GLThread *glThread;
    static YUVRenderer *render;

    ANativeWindow *surface = nullptr;
    pthread_t threadInit;
    pthread_t threadDemuxer;
    pthread_mutex_t mutexInit;
    pthread_mutex_t mutexSeek;

    AVFormatContext *pAVFormatContextInput = nullptr;

    RenderMode renderMode = RENDERMODE_CONTINUOUSLY;

    bool HWSupport = false;

    HWDecoder *decoder = nullptr;


public:
    MediaController(PlayState *ps, Callback2Java *cj, Audio *a, Video *v);

    ~MediaController();

    void initAudio(int streamIndex);

    void initVideo(int streamIndex);

    void initSubtitle(int streamIndex);

    int getAVCodecContext(AVCodecParameters *param, AVCodecContext **codecContext);

    void setMimeType(string type);

    void setHWSupport(bool s);

public://surface - egl - lifecycle stuff

    void surfaceCreate(ANativeWindow *surface);

    void surfaceChange(int width, int height);

    void surfaceDraw();

    void setRenderMode(RenderMode mode);


public://media control stuff

    void prepare(const char *url, bool supportHW);

    void start();

    void pause();

    void resume();

    void stop();

    void seekTo(long second);

    void release();

public://thread callbacks

    void threadCallbackMediaInitialize();

    void threadCallbackMediaDemuxer();

private://decode stuff

    static void callbackYUVData(int width, int height, void *y, void *u, void *v);

    static void callbackHWData(void *data, int size);

};


#endif //MEDIAPLAYER_FFMPEG_MEDIACONTROLLER_H
