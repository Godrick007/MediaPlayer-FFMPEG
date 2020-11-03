#include "jni.h"
#include "util/LogUtil.h"
#include "Callback2Java.h"

Callback2Java *cb2j = nullptr;
JavaVM *javaVM;

void _prepare(JNIEnv *env, jobject instance, jstring url);
void _setSurface(JNIEnv *env, jobject instance, jobject surface);

static const JNINativeMethod methods[] = {
        {"nPrepare", "(Ljava/lang/String;)V", (void *) _prepare},
        {"nSetSurface", "(Landroid/view/Surface;)V", (void *) _setSurface}
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


void _prepare(JNIEnv *env, jobject instance, jstring url) {
    const char *_url = env->GetStringUTFChars(url, nullptr);
    LOGE("MediaPlayer", "test for register method %s", _url);





    env->ReleaseStringUTFChars(url, _url);
}

void _setSurface(JNIEnv *env, jobject instance, jobject surface){

}







