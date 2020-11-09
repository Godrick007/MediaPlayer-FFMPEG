//
// Created by Godrick Crown on 2020/10/30.
//

#ifndef MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H
#define MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H

#include <jni.h>
#include "util/LogUtil.h"

enum CallbackThread {
    MAIN_THREAD,
    WORK_THREAD
};

class Callback2Java {

    /**
     * method ids
     */
public:
    jmethodID mid_OnJNILoadFailed;
    jmethodID mid_JniMethodRegisterError;
    jmethodID mid_MediaPlayerInitError;
    jmethodID mid_MediaPlayerPrepared;
    jmethodID mid_MediaPlayerComplete;
    jmethodID mid_MediaPlayerLoading;

    /**
     * members
     */
public:
    JavaVM *jvm;
    JNIEnv *env;
    jobject jobj;

    /**
     * lifecycle
     */
public:
    Callback2Java(JavaVM *jvm, JNIEnv *env, jobject jobj);

    ~Callback2Java();

    /**
     *  JNI
     */
public:
    void cb2j_JNI_OnLoadFailed(CallbackThread thread);

    void cb2j_JNI_MethodFailed(CallbackThread thread);

    /**
     *  Media
     */
public:

    void cb2j_MediaPlayer_InitError(CallbackThread thread, int errorCode, const char *msg);

    void cb2j_MediaPlayer_Prepared(CallbackThread thread);

    void cb2j_MediaPlayer_Complete(CallbackThread thread);

    void cb2j_MediaPlayer_Loading(CallbackThread thread, bool loading);
};


#endif //MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H
