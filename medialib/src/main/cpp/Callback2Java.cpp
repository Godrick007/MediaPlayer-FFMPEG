//
// Created by Godrick Crown on 2020/10/30.
//

#include "Callback2Java.h"

Callback2Java::Callback2Java(JavaVM *jvm, JNIEnv *env, jobject jobj) {

    this->jvm = jvm;
    this->env = env;
    this->jobj = env->NewGlobalRef(jobj);

    jclass clz = env->GetObjectClass(jobj);

    if (!clz) {
        if (LOG_DEBUG) {
            LOGE("MediaPlayer", "Get jclass error!\n");
        }
        return;
    }

    mid_OnJNILoadFailed = env->GetMethodID(clz, "cb_OnJniLoadFailed", "()V");
    mid_JniMethodRegisterError = env->GetMethodID(clz, "cb_JniMethodRegisterError", "()V");
    mid_MediaPlayerInitError = env->GetMethodID(clz, "cb_MediaPlayerInitError",
                                                "(ILjava/lang/String;)V");
    mid_MediaPlayerPrepared = env->GetMethodID(clz, "cb_MediaPlayerPrepared", "()V");
    mid_MediaPlayerComplete = env->GetMethodID(clz, "cb_MediaPlayerComplete", "()V");
    mid_MediaPlayerLoading = env->GetMethodID(clz, "cb_MediaPlayerLoading", "(Z)V");
    mid_MediaPlayerLoading = env->GetMethodID(clz, "cb_MediaPlayerProgress", "(JJ)V");
    mid_MediaPlayerDBValue = env->GetMethodID(clz, "cb_MediaPlayerDBValue", "(D)V");
    mid_MediaPlayerSLInitError = env->GetMethodID(clz, "cb_MediaPlayerSLInitError",
                                                  "(ILjava/lang/String;)V");
}

Callback2Java::~Callback2Java() = default;

void tmp(CallbackThread thread) {
    if (thread == MAIN_THREAD) {

        //this->jniEnv->CallVoidMethod(jobj, mid_prepared);

    } else {
//    JNIEnv *env;
//    if(jvm->AttachCurrentThread(&env, 0 )!= JNI_OK){
//        if(LOG_DEBUG){
//            LOGD("Ffmpeg","get thread jniEnv error");
//        }
//        return;
//    }
//
//    env->CallVoidMethod(this->jobj, mid);
//
//    jvm->DetachCurrentThread();
    }


}

void Callback2Java::cb2j_JNI_OnLoadFailed(CallbackThread thread) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(this->jobj, mid_OnJNILoadFailed);
    } else {
        JNIEnv *env = nullptr;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "Get thread jniEnv error");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_OnJNILoadFailed);
        jvm->DetachCurrentThread();
    }
}

void Callback2Java::cb2j_JNI_MethodFailed(CallbackThread thread) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(this->jobj, mid_JniMethodRegisterError);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "Get thread jniEnv error");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_JniMethodRegisterError);
        jvm->DetachCurrentThread();
    }
}

void
Callback2Java::cb2j_MediaPlayer_InitError(CallbackThread thread, int errorCode, const char *msg) {
    jstring str = nullptr;
    if (thread == MAIN_THREAD) {
        str = this->env->NewStringUTF(msg);
        this->env->CallVoidMethod(this->jobj, mid_MediaPlayerInitError, errorCode, str);
        env->ReleaseStringUTFChars(str, msg);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "Player init error code = %d, msg = %s", errorCode, msg);
            }
            return;
        }
        str = env->NewStringUTF(msg);
        env->CallVoidMethod(this->jobj, mid_MediaPlayerInitError, errorCode, str);
        env->DeleteLocalRef(str);
        jvm->DetachCurrentThread();
    }

}

void Callback2Java::cb2j_MediaPlayer_Prepared(CallbackThread thread) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(jobj, mid_MediaPlayerPrepared);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "media prepared");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_MediaPlayerPrepared);
        jvm->DetachCurrentThread();
    }

}

void Callback2Java::cb2j_MediaPlayer_Complete(CallbackThread thread) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(jobj, mid_MediaPlayerComplete);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "media complete");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_MediaPlayerComplete);
        jvm->DetachCurrentThread();
    }

}

void Callback2Java::cb2j_MediaPlayer_Loading(CallbackThread thread, bool loading) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(jobj, mid_MediaPlayerLoading, loading);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "media loading");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_MediaPlayerLoading, loading);
        jvm->DetachCurrentThread();
    }
}

void Callback2Java::cb2j_MediaPlayer_Progress(CallbackThread thread, long current, long duration) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(jobj, mid_MediaPlayerProgress, current, duration);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            return;
        }
//        if (LOG_DEBUG) {
//            LOGD("MediaPlayer", "cb2j_MediaPlayer_Progress current is %ld,duration is %ld",
//                 current, duration);
//        }
//        LOGD("MediaPlayer", "cb2j_MediaPlayer_Progress current is %ld,duration is %ld",
//             current, duration);
        env->CallVoidMethod(this->jobj, mid_MediaPlayerProgress, current, duration);
        jvm->DetachCurrentThread();
    }
}

void Callback2Java::cb2j_MediaPlayer_DBValue(CallbackThread thread, double dbValue) {
    if (thread == MAIN_THREAD) {
        this->env->CallVoidMethod(jobj, mid_MediaPlayerDBValue, dbValue);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                //LOGD("MediaPlayer", "media loading");
            }
            return;
        }
        env->CallVoidMethod(this->jobj, mid_MediaPlayerDBValue, dbValue);
        jvm->DetachCurrentThread();
    }
}

void
Callback2Java::cb2j_MediaPlayer_SL_InitError(CallbackThread thread, int errorCode,
                                             const char *msg) {
    jstring str = nullptr;
    if (thread == MAIN_THREAD) {
        str = this->env->NewStringUTF(msg);
        this->env->CallVoidMethod(this->jobj, mid_MediaPlayerSLInitError, errorCode, str);
        env->ReleaseStringUTFChars(str, msg);
    } else {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGD("MediaPlayer", "Player init error code = %d, msg = %s", errorCode, msg);
            }
            return;
        }
        str = env->NewStringUTF(msg);
        env->CallVoidMethod(this->jobj, mid_MediaPlayerSLInitError, errorCode, str);
        env->DeleteLocalRef(str);
        jvm->DetachCurrentThread();
    }
}


