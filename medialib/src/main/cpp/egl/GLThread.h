//
// Created by Godrick Crown on 2021/1/21.
//

#ifndef MEDIAPLAYER_FFMPEG_GLTHREAD_H
#define MEDIAPLAYER_FFMPEG_GLTHREAD_H

#include <pthread.h>
#include <android/native_window.h>
#include "GLESRenderer.h"
#include "EGLHelper.h"
#include "../render/YUVRenderer.h"
#include <unistd.h>

class GLThread {

private:

    bool exit = false;
    bool create = false;
    bool changed = false;
    bool firstIn = false;

    ANativeWindow *win;

    YUVRenderer *renderer;


    int width;
    int height;

    pthread_t thread_run;
    pthread_mutex_t mutex_lock;
    pthread_cond_t cond_draw;
    EGLHelper *eglHelper = nullptr;
    EGLContext eglContext;

    int renderMode = 0;

public:

    GLThread(ANativeWindow *win);
    ~GLThread();

    void start();

    void run();

    void surfaceCreate();

    void surfaceChange(int width, int height);

    void surfaceDraw();

    void requestRender();

    void setChanged(int width, int height);

    void setRenderMode(int mode);

    void setRenderer(YUVRenderer *renderer);

};


#endif //MEDIAPLAYER_FFMPEG_GLTHREAD_H
