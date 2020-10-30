//
// Created by Godrick Crown on 2020/10/30.
//

#ifndef MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H
#define MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H

#include <jni.h>
#include "LogUtil.h"

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

};


#endif //MEDIAPLAYER_FFMPEG_CALLBACK2JAVA_H
