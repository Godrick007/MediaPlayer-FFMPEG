#include "jni.h"
#include "util/LogUtil.h"
#include "Callback2Java.h"
#include "decode/MediaPlayer.h"
#include "decode/PlayState.h"

JavaVM *javaVM = nullptr;
Callback2Java *pCb2j = nullptr;
PlayState *pPlayState = nullptr;
MediaPlayer *pMediaPlayer = nullptr;

void _setSurface(JNIEnv *env, jobject instance, jobject surface);

void _prepare(JNIEnv *env, jobject instance, jstring url, jboolean isLiving);

void _start(JNIEnv *env, jobject instance);

void _pause(JNIEnv *env, jobject instance);

void _resume(JNIEnv *env, jobject instance);

void _stop(JNIEnv *env, jobject instance);

void _release(JNIEnv *env, jobject instance);

void _setSpeed(JNIEnv *env, jobject instance, jfloat speed);

static const JNINativeMethod methods[] = {
        //renderer
        {"nSetSurface",   "(Landroid/view/Surface;)V", (void *) _setSurface},

        //media player controller
        {"nPrepare",      "(Ljava/lang/String;Z)V",    (void *) _prepare},
        {"nStart",        "()V",                       (void *) _start},
        {"nPause",        "()V",                       (void *) _pause},
        {"nResume",       "()V",                       (void *) _resume},
        {"nStop",         "()V",                       (void *) _stop},
        {"nRelease",      "()V",                       (void *) _release},
        {"nSetPlaySpeed", "(F)V",                      (void *) _setSpeed},
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
        pMediaPlayer = new MediaPlayer(pPlayState, pCb2j, _url, isLiving);
    }


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

void _setSpeed(JNIEnv *env, jobject instance, jfloat speed) {
    if (pMediaPlayer) {
        pMediaPlayer->setSpeed(speed);
    }
}








