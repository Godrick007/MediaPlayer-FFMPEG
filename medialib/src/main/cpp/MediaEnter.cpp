#include "jni.h"
#include "util/LogUtil.h"
#include "Callback2Java.h"
#include "decode/MediaPlayer.h"
#include "decode/PlayState.h"
#include "egl/GLESRenderer.h"
#include "egl/EGLHelper.h"
#include <android/native_window_jni.h>
#include <unistd.h>


JavaVM *javaVM = nullptr;
Callback2Java *pCb2j = nullptr;
PlayState *pPlayState = nullptr;
MediaPlayer *pMediaPlayer = nullptr;
Renderer *pRenderer = nullptr;


void _setSurface(JNIEnv *env, jobject instance, jobject surface);

void _prepare(JNIEnv *env, jobject instance, jstring url, jboolean isLiving);

void _start(JNIEnv *env, jobject instance);

void _pause(JNIEnv *env, jobject instance);

void _resume(JNIEnv *env, jobject instance);

void _stop(JNIEnv *env, jobject instance);

void _release(JNIEnv *env, jobject instance);

void _setSpeed(JNIEnv *env, jobject instance, jfloat speed);

void _rendererInit(JNIEnv *env, jobject instance);

void _rendererResize(JNIEnv *env, jobject instance, jint width, jint height);

void _rendererDrawFrame(JNIEnv *env, jobject instance);

void _eglStart(JNIEnv *env, jobject instance, jobject surface);


static const JNINativeMethod methods[] = {
        //renderer
        {"nSetSurface",         "(Landroid/view/Surface;)V", (void *) _setSurface},

        //media player controller
        {"nPrepare",            "(Ljava/lang/String;Z)V",    (void *) _prepare},
        {"nStart",              "()V",                       (void *) _start},
        {"nPause",              "()V",                       (void *) _pause},
        {"nResume",             "()V",                       (void *) _resume},
        {"nStop",               "()V",                       (void *) _stop},
        {"nRelease",            "()V",                       (void *) _release},
        {"nSetPlaySpeed",       "(F)V",                      (void *) _setSpeed},
        {"nRendererInitialize", "()V",                       (void *) _rendererInit},
        {"nRendererResize",     "(II)V",                     (void *) _rendererResize},
        {"nRendererDrawFrame",  "()V",                       (void *) _rendererDrawFrame},
        {"nEGLStart",           "(Landroid/view/Surface;)V", (void *) _eglStart},
};


extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE("MediaPlayer", "JNI_OnLoad get JNIEnv error");
        return -1;
    }

    javaVM = vm;

    jclass clz = env->FindClass("godrick/media/medialib/natives/NativeMediaEnter");

    if (env->RegisterNatives(clz, methods, sizeof(methods) / sizeof(methods[0])) != JNI_OK) {
        LOGE("MediaPlayer", "JNI_OnLoad register native error");
        return -1;
    }

    return JNI_VERSION_1_6;
}


void _setSurface(JNIEnv *env, jobject instance, jobject surface) {

}

void _prepare(JNIEnv *env, jobject instance, jstring url, jboolean isLiving) {
    const char *_url = env->GetStringUTFChars(url, nullptr);
    LOGE("MediaPlayer", "Open media file %s", _url);

    if (!pCb2j) {
        pCb2j = new Callback2Java(javaVM, env, instance);
    }

    if (!pPlayState) {
        pPlayState = new PlayState();
    }

    if (!pMediaPlayer) {
        pMediaPlayer = new MediaPlayer(pPlayState, pCb2j, _url, pRenderer, isLiving);
    } else {
        pMediaPlayer->setUrl(_url);
        if (pMediaPlayer->render == nullptr) {
            pMediaPlayer->setRenderer(pRenderer);
        }

    }
    LOGD("MediaPlayer", "renderer is %s", pRenderer == nullptr ? "null" : "not null");
    pMediaPlayer->prepare();

//    if (pMediaPlayer && pRenderer) {
//        pMediaPlayer->setRenderer(pRenderer);
//    }
//    env->ReleaseStringUTFChars(url, _url);
}

void _start(JNIEnv *env, jobject instance) {

    if (pMediaPlayer) {
        pMediaPlayer->play();
    }

}

void _pause(JNIEnv *env, jobject instance) {
    if (pMediaPlayer) {
        pMediaPlayer->pause();
    }
}

void _resume(JNIEnv *env, jobject instance) {
    if (pMediaPlayer) {
        pMediaPlayer->resume();
    }
}

void _stop(JNIEnv *env, jobject instance) {
    if (pMediaPlayer) {
        pMediaPlayer->release();
        delete pMediaPlayer;
        pMediaPlayer = nullptr;
    }
    if (pCb2j) {
        delete pCb2j;
        pCb2j = nullptr;
    }
    if (pPlayState) {
        delete pPlayState;
        pPlayState = nullptr;
    }
}

void _release(JNIEnv *env, jobject instance) {
    if (pMediaPlayer) {
//        pMediaPlayer->release();
        delete pMediaPlayer;
        pMediaPlayer = nullptr;
    }
    if (pCb2j) {
        delete pCb2j;
        pCb2j = nullptr;
    }
    if (pPlayState) {
        delete pPlayState;
        pPlayState = nullptr;
    }

}

void _setSpeed(JNIEnv *env, jobject instance, jfloat speed) {
    if (pMediaPlayer) {
        pMediaPlayer->setSpeed(speed);
    }
}


void _rendererInit(JNIEnv *env, jobject instance) {
    if (pRenderer != nullptr) {
        delete pRenderer;
        pRenderer = nullptr;
    }

    if (LOG_DEBUG) {
//        pRenderer->printGLString("Version", GL_VERSION);
//        pRenderer->printGLString("Vendor", GL_VENDOR);
//        pRenderer->printGLString("Renderer", GL_RENDERER);
//        pRenderer->printGLString("Extensions", GL_EXTENSIONS);
    }

//    const char *versionStr = reinterpret_cast<const char *>(glGetString(GL_VERSION));
//    if (strstr(versionStr, "OpenGL ES 3.")) {
//        //todo
//
//    } else if (strstr(versionStr, "OpenGL ES 2.")) {
//        pRenderer = createES2Renderer();
//    } else {
//
//    }
    pRenderer = createES2Renderer();

    if (!pCb2j) {
        pCb2j = new Callback2Java(javaVM, env, instance);
    }

    if (!pPlayState) {
        pPlayState = new PlayState();
    }

    if (!pMediaPlayer) {
        pMediaPlayer = new MediaPlayer(pPlayState, pCb2j, pRenderer);
    }
    LOGI("MediaPlayer", "initSW renderer is %s", pRenderer == nullptr ? "null" : "not null");
//    if (pMediaPlayer && pRenderer) {
//        pMediaPlayer->setRenderer(pRenderer);
//    }

}

void _rendererResize(JNIEnv *env, jobject instance, jint width, jint height) {
    if (pRenderer) {
        pRenderer->resize(width, height);
    }
}

void _rendererDrawFrame(JNIEnv *env, jobject instance) {
    if (pRenderer) {
        pRenderer->render();
    }
}


void *eglStartCallback(void *context) {

    EGLHelper *eglHelper = new EGLHelper();
    if (eglHelper->start(static_cast<ANativeWindow *>(context), nullptr)) {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "start() success");
        }
        while (1) {

            glViewport(0, 0, 100, 100);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(1.0, 1.0, 0, 0);


            eglHelper->swap();
            usleep(16 * 1000);
        }
    } else {
        if (LOG_DEBUG) {
            LOGI("EGLHelper", "start() failed");
        }
    }
    return nullptr;
}

void _eglStart(JNIEnv *env, jobject instance, jobject surface) {

    pthread_t threadEGLStart;

//    1.如果在UI线程中创建SurfaceView，那么需要在UI线程中使用ANativeWindow_fromSurface；
//
//    2.如果在非UI线程中创建SurfaceView，那么需要等待SurfaceView创建完成之后，才能调用ANativeWindow_fromSurface，至于能不能在UI线程中调用ANativeWindow_fromSurface，那我就没试过了，有朋友试过的话，不妨告诉我。

    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);

    pthread_create(&threadEGLStart, nullptr, eglStartCallback, win);


}








