//
// Created by Administrator on 2016/9/2.
//

#define LOG_NDEBUG 1
#define LOG_TAG "YTX-PLAYER-JNI"
#include "android_media_YtxMediaPlayer.h"
// 获取数组的大小
#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

// 指定要注册的类，对应完整的java类名
//#define JNIREG_CLASS "com/example/aecm/MobileAEC"

#define JNIREG_CLASS "com/ytx/ican/media/player/YtxMediaPlayer"


JNIEXPORT void JNICALL android_media_player_native_init
        (JNIEnv *env, jclass obj)
{

}

JNIEXPORT void JNICALL android_media_player_native_setup
        (JNIEnv *env, jobject obj, jobject ytxMediaPlayer)
{

}

JNIEXPORT void JNICALL android_media_player_native_finalize
        (JNIEnv *env, jobject obj)
{

}

JNIEXPORT void JNICALL android_media_player_native_message_loop
        (JNIEnv *env, jobject obj, jobject ytxMediaPlayer)
{

}

// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {

        {"native_init",      "()V",                              (void *)android_media_player_native_init},
        {"native_setup",        "(Ljava/lang/Object;)V",         (void *)android_media_player_native_setup},
        {"native_finalize",  "()V",                              (void *)android_media_player_native_finalize},
        {"native_message_loop", "(Ljava/lang/Object;)V",         (void *)android_media_player_native_message_loop},

};



// 注册native方法到java中
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}


int register_android_media_player_ytx(JNIEnv *env)
{
    // 调用注册方法
    return registerNativeMethods(env, JNIREG_CLASS,
                                 gMethods, NELEM(gMethods));
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);


    if (register_android_media_player_ytx(env) < 0) {
        ALOGE("ERROR: YTX mediaPlayer native registration failed\n");
        goto bail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}