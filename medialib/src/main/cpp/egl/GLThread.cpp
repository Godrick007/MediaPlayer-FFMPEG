//
// Created by Godrick Crown on 2021/1/21.
//


#include "GLThread.h"

GLThread::GLThread(ANativeWindow *win) {
    this->win = win;
    pthread_mutex_init(&mutex_lock, nullptr);
    pthread_cond_init(&cond_draw, nullptr);
}


void *startCallback(void *ctx) {
    auto *instance = static_cast<GLThread *>(ctx);
    instance->run();
    return nullptr;
}

void GLThread::start() {
    pthread_create(&thread_run, nullptr, startCallback, this);
}

void GLThread::run() {

    eglHelper = new EGLHelper();
    eglHelper->start(win, eglContext);
    exit = false;
    firstIn = true;
    create = false;
    changed = false;

    while (true) {
        pthread_mutex_lock(&mutex_lock);
        if (exit) {
            break;
        }

        if (!firstIn) {
            firstIn = false;
            if (renderMode == 0) {
                usleep(1000 / 60 * 1000);
            } else if (renderMode == 1) {
                pthread_cond_wait(&cond_draw, &mutex_lock);
            } else {

            }
        }
        if (!create) {
            surfaceCreate();
            create = true;
        }

        if (!changed) {
//            LOGI("c_tag", "GLThread::run surfaceChange width = %d,height=%d", width, height);
            surfaceChange(width, height);
            changed = true;
        }

        surfaceDraw();

        eglHelper->swap();

        pthread_mutex_unlock(&mutex_lock);
    }

}

void GLThread::surfaceCreate() {
    //notify the renderer of that surface has created
    LOGI("c_tag", "GLThread::surfaceCreate");
    if (renderer != nullptr) {
        renderer->init();
    }
}

void GLThread::surfaceChange(int width, int height) {
    //notify the renderer of that surface has changed
    LOGI("c_tag", "GLThread::surfaceChange renderer = %d", renderer != nullptr);
    if (renderer != nullptr) {
        renderer->surfaceChanged(width, height);
    }
}

void GLThread::surfaceDraw() {
    //notify the renderer to draw
    if (renderer != nullptr) {
        renderer->drawYUV();
    }
}

void GLThread::requestRender() {
    pthread_cond_signal(&cond_draw);
}

void GLThread::setChanged(int width, int height) {
    LOGI("c_tag", "GLThread::setChanged");
    changed = false;
    this->width = width;
    this->height = height;
}

void GLThread::setRenderMode(int mode) {
    renderMode = mode;
}

void GLThread::setRenderer(YUVRenderer *renderer) {
    this->renderer = renderer;
}



