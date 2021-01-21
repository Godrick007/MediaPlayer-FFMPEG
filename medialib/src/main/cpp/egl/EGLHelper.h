//
// Created by Godrick Crown on 2021/1/18.
//

#ifndef MEDIAPLAYER_FFMPEG_EGLHELPER_H
#define MEDIAPLAYER_FFMPEG_EGLHELPER_H

#include <EGL/egl.h>
#include "../util/LogUtil.h"
#include <pthread.h>

const EGLint attrSurface[] = {EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES2_BIT,  // Request opengl ES2.0
                              EGL_BLUE_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_RED_SIZE,
                              8,
                              EGL_DEPTH_SIZE,
                              16,
                              EGL_NONE};

const EGLint attrContext[] = {EGL_CONTEXT_CLIENT_VERSION,
                              2,  // Request opengl ES2.0
                              EGL_NONE};

class EGLHelper {

private:
    ANativeWindow *mSurface;
    EGLContext mEglContext;
    EGLSurface mEglSurface;
    EGLDisplay mEglDisplay;
    EGLConfig mEglConfig;

    int color_size;
    int depth_size;


public:

    bool start(ANativeWindow *win, EGLContext eglContext);

    int swap();

    EGLContext getEglContext();

    void destroyEgl();

    int findConfigAttrib(EGLConfig config, int attr, int defaultValue);

};


#endif //MEDIAPLAYER_FFMPEG_EGLHELPER_H
