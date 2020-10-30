#include "jni.h"
#include "LogUtil.h"
#include "Callback2Java.h"

Callback2Java *cb2j = NULL;
JavaVM *javaVM;

void _prepare(JNIEnv *env, jobject instance, jstring url);

static const JNINativeMethod methods[] = {
        {"nPrepare", "(Ljava/lang/String;)V", (void *) _prepare}
};


extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
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


void _prepare(JNIEnv *env, jobject instance, jstring url) {
    const char *_url = env->GetStringUTFChars(url, 0);
    LOGE("MediaPlayer", "test for register method %s", _url);
    env->ReleaseStringUTFChars(url, _url);
}









