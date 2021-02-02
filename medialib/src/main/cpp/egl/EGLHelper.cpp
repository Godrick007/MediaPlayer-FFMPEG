//
// Created by Godrick Crown on 2021/1/18.
//

#include "EGLHelper.h"


bool EGLHelper::start(ANativeWindow *win, EGLContext eglContext) {


    if (LOG_DEBUG) {
        LOGI("EGLHelper", "start(),tid = %ld", pthread_self());
    }

    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (mEglDisplay == EGL_NO_DISPLAY) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "eglGetDisplay failed");
        }
        return false;
    }


    if (!eglInitialize(mEglDisplay, 0, 0)) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "eglInitialize failed");
        }
        return false;
    }

    color_size = 8;
    depth_size = 16;


    EGLint num_configs;

    if (!eglChooseConfig(mEglDisplay, attrSurface, &mEglConfig, 1, &num_configs)) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "eglChooseConfig#1 failed");
        }

        return false;
    }

    if (!num_configs) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "No configs match configSpec");
        }
        return false;
    }

    mEglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig, win, nullptr);

    if (mEglSurface == nullptr || mEglSurface == EGL_NO_SURFACE) {

        int error = eglGetError();
        if (error == EGL_BAD_NATIVE_WINDOW) {
            if (LOG_DEBUG) {
                LOGI("EGLHelper", "eglCreateWindowSurface returned EGL_BAD_NATIVE_WINDOW");
            }
        }
        return false;
    }

    mEglContext = eglCreateContext(mEglDisplay, mEglConfig,
                                   nullptr,
                                   attrContext);

    if (mEglContext == nullptr) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "eglCreateContext failed");
        }
        return false;
    }


    if (!eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "eglMakeCurrent failed");
        }
        return false;
    }


    return true;
}

int EGLHelper::swap() {
    if (!eglSwapBuffers(mEglDisplay, mEglSurface)) {
        return eglGetError();
    }
    return EGL_SUCCESS;
}

EGLContext EGLHelper::getEglContext() {
    return mEglContext;
}

void EGLHelper::destroyEgl() {
    eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(mEglDisplay, mEglContext);
    eglDestroySurface(mEglDisplay, mEglSurface);
    eglTerminate(mEglDisplay);
}

int EGLHelper::findConfigAttrib(EGLConfig config, int attr, int defaultValue) {

    EGLint values[] = {0};

    if (eglGetConfigAttrib(mEglDisplay, config, attr, values)) {
        return values[0];
    }

    return defaultValue;
}
