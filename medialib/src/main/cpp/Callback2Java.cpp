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
