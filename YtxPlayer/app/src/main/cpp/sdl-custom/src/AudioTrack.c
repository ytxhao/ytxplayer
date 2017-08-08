//
// Created by Administrator on 2017/7/18.
//
#include "../include/AudioTrack.h"

typedef struct J4AC_android_media_AudioTrack {
    jclass id;

    jmethodID constructor_AudioTrack;
    jmethodID method_getMinBufferSize;
    jmethodID method_getMaxVolume;
    jmethodID method_getMinVolume;
    jmethodID method_getNativeOutputSampleRate;
    jmethodID method_play;
    jmethodID method_pause;
    jmethodID method_stop;
    jmethodID method_flush;
    jmethodID method_release;
    jmethodID method_write;
    jmethodID method_setStereoVolume;
    jmethodID method_getAudioSessionId;
    jmethodID method_getPlaybackParams;
    jmethodID method_setPlaybackParams;
    jmethodID method_getStreamType;
    jmethodID method_getSampleRate;
    jmethodID method_getPlaybackRate;
    jmethodID method_setPlaybackRate;
} J4AC_android_media_AudioTrack;
static J4AC_android_media_AudioTrack class_J4AC_android_media_AudioTrack;

jobject J4AC_android_media_AudioTrack__AudioTrack(JNIEnv *env, jint streamType, jint sampleRateInHz, jint channelConfig, jint audioFormat, jint bufferSizeInBytes, jint mode)
{
    return (*env)->NewObject(env, class_J4AC_android_media_AudioTrack.id, class_J4AC_android_media_AudioTrack.constructor_AudioTrack, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
}

jobject J4AC_android_media_AudioTrack__AudioTrack__catchAll(JNIEnv *env, jint streamType, jint sampleRateInHz, jint channelConfig, jint audioFormat, jint bufferSizeInBytes, jint mode)
{
    jobject ret_object = J4AC_android_media_AudioTrack__AudioTrack(env, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_android_media_AudioTrack__AudioTrack__asGlobalRef__catchAll(JNIEnv *env, jint streamType, jint sampleRateInHz, jint channelConfig, jint audioFormat, jint bufferSizeInBytes, jint mode)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_android_media_AudioTrack__AudioTrack__catchAll(env, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
    if (J4A_ExceptionCheck__catchAll(env) || !local_object) {
        ret_object = NULL;
        goto fail;
    }

    ret_object = J4A_NewGlobalRef__catchAll(env, local_object);
    if (!ret_object) {
        ret_object = NULL;
        goto fail;
    }

    fail:
    J4A_DeleteLocalRef__p(env, &local_object);
    return ret_object;
}

jint J4AC_android_media_AudioTrack__getMinBufferSize(JNIEnv *env, jint sampleRateInHz, jint channelConfig, jint audioFormat)
{
    return (*env)->CallStaticIntMethod(env, class_J4AC_android_media_AudioTrack.id, class_J4AC_android_media_AudioTrack.method_getMinBufferSize, sampleRateInHz, channelConfig, audioFormat);
}

jint J4AC_android_media_AudioTrack__getMinBufferSize__catchAll(JNIEnv *env, jint sampleRateInHz, jint channelConfig, jint audioFormat)
{
    jint ret_value = J4AC_android_media_AudioTrack__getMinBufferSize(env, sampleRateInHz, channelConfig, audioFormat);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jfloat J4AC_android_media_AudioTrack__getMaxVolume(JNIEnv *env)
{
    return (*env)->CallStaticFloatMethod(env, class_J4AC_android_media_AudioTrack.id, class_J4AC_android_media_AudioTrack.method_getMaxVolume);
}

jfloat J4AC_android_media_AudioTrack__getMaxVolume__catchAll(JNIEnv *env)
{
    jfloat ret_value = J4AC_android_media_AudioTrack__getMaxVolume(env);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jfloat J4AC_android_media_AudioTrack__getMinVolume(JNIEnv *env)
{
    return (*env)->CallStaticFloatMethod(env, class_J4AC_android_media_AudioTrack.id, class_J4AC_android_media_AudioTrack.method_getMinVolume);
}

jfloat J4AC_android_media_AudioTrack__getMinVolume__catchAll(JNIEnv *env)
{
    jfloat ret_value = J4AC_android_media_AudioTrack__getMinVolume(env);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__getNativeOutputSampleRate(JNIEnv *env, jint streamType)
{
    return (*env)->CallStaticIntMethod(env, class_J4AC_android_media_AudioTrack.id, class_J4AC_android_media_AudioTrack.method_getNativeOutputSampleRate, streamType);
}

jint J4AC_android_media_AudioTrack__getNativeOutputSampleRate__catchAll(JNIEnv *env, jint streamType)
{
    jint ret_value = J4AC_android_media_AudioTrack__getNativeOutputSampleRate(env, streamType);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_android_media_AudioTrack__play(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_play);
}

void J4AC_android_media_AudioTrack__play__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_android_media_AudioTrack__play(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_android_media_AudioTrack__pause(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_pause);
}

void J4AC_android_media_AudioTrack__pause__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_android_media_AudioTrack__pause(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_android_media_AudioTrack__stop(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_stop);
}

void J4AC_android_media_AudioTrack__stop__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_android_media_AudioTrack__stop(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_android_media_AudioTrack__flush(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_flush);
}

void J4AC_android_media_AudioTrack__flush__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_android_media_AudioTrack__flush(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_android_media_AudioTrack__release(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_release);
}

void J4AC_android_media_AudioTrack__release__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_android_media_AudioTrack__release(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_android_media_AudioTrack__write(JNIEnv *env, jobject thiz, jbyteArray audioData, jint offsetInBytes, jint sizeInBytes)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_write, audioData, offsetInBytes, sizeInBytes);
}

jint J4AC_android_media_AudioTrack__write__catchAll(JNIEnv *env, jobject thiz, jbyteArray audioData, jint offsetInBytes, jint sizeInBytes)
{
    jint ret_value = J4AC_android_media_AudioTrack__write(env, thiz, audioData, offsetInBytes, sizeInBytes);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__setStereoVolume(JNIEnv *env, jobject thiz, jfloat leftGain, jfloat rightGain)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_setStereoVolume, leftGain, rightGain);
}

jint J4AC_android_media_AudioTrack__setStereoVolume__catchAll(JNIEnv *env, jobject thiz, jfloat leftGain, jfloat rightGain)
{
    jint ret_value = J4AC_android_media_AudioTrack__setStereoVolume(env, thiz, leftGain, rightGain);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__getAudioSessionId(JNIEnv *env, jobject thiz)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_getAudioSessionId);
}

jint J4AC_android_media_AudioTrack__getAudioSessionId__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_android_media_AudioTrack__getAudioSessionId(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jobject J4AC_android_media_AudioTrack__getPlaybackParams(JNIEnv *env, jobject thiz)
{
    return (*env)->CallObjectMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_getPlaybackParams);
}

jobject J4AC_android_media_AudioTrack__getPlaybackParams__catchAll(JNIEnv *env, jobject thiz)
{
    jobject ret_object = J4AC_android_media_AudioTrack__getPlaybackParams(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_android_media_AudioTrack__getPlaybackParams__asGlobalRef__catchAll(JNIEnv *env, jobject thiz)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_android_media_AudioTrack__getPlaybackParams__catchAll(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !local_object) {
        ret_object = NULL;
        goto fail;
    }

    ret_object = J4A_NewGlobalRef__catchAll(env, local_object);
    if (!ret_object) {
        ret_object = NULL;
        goto fail;
    }

    fail:
    J4A_DeleteLocalRef__p(env, &local_object);
    return ret_object;
}

void J4AC_android_media_AudioTrack__setPlaybackParams(JNIEnv *env, jobject thiz, jobject params)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_setPlaybackParams, params);
}

void J4AC_android_media_AudioTrack__setPlaybackParams__catchAll(JNIEnv *env, jobject thiz, jobject params)
{
    J4AC_android_media_AudioTrack__setPlaybackParams(env, thiz, params);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_android_media_AudioTrack__getStreamType(JNIEnv *env, jobject thiz)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_getStreamType);
}

jint J4AC_android_media_AudioTrack__getStreamType__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_android_media_AudioTrack__getStreamType(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__getSampleRate(JNIEnv *env, jobject thiz)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_getSampleRate);
}

jint J4AC_android_media_AudioTrack__getSampleRate__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_android_media_AudioTrack__getSampleRate(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__getPlaybackRate(JNIEnv *env, jobject thiz)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_getPlaybackRate);
}

jint J4AC_android_media_AudioTrack__getPlaybackRate__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_android_media_AudioTrack__getPlaybackRate(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jint J4AC_android_media_AudioTrack__setPlaybackRate(JNIEnv *env, jobject thiz, jint sampleRateInHz)
{
    return (*env)->CallIntMethod(env, thiz, class_J4AC_android_media_AudioTrack.method_setPlaybackRate, sampleRateInHz);
}

jint J4AC_android_media_AudioTrack__setPlaybackRate__catchAll(JNIEnv *env, jobject thiz, jint sampleRateInHz)
{
    jint ret_value = J4AC_android_media_AudioTrack__setPlaybackRate(env, thiz, sampleRateInHz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

int J4A_loadClass__J4AC_android_media_AudioTrack(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_android_media_AudioTrack.id != NULL)
        return 0;

    sign = "android/media/AudioTrack";
    class_J4AC_android_media_AudioTrack.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_android_media_AudioTrack.id == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "<init>";
    sign     = "(IIIIII)V";
    class_J4AC_android_media_AudioTrack.constructor_AudioTrack = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.constructor_AudioTrack == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getMinBufferSize";
    sign     = "(III)I";
    class_J4AC_android_media_AudioTrack.method_getMinBufferSize = J4A_GetStaticMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getMinBufferSize == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getMaxVolume";
    sign     = "()F";
    class_J4AC_android_media_AudioTrack.method_getMaxVolume = J4A_GetStaticMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getMaxVolume == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getMinVolume";
    sign     = "()F";
    class_J4AC_android_media_AudioTrack.method_getMinVolume = J4A_GetStaticMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getMinVolume == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getNativeOutputSampleRate";
    sign     = "(I)I";
    class_J4AC_android_media_AudioTrack.method_getNativeOutputSampleRate = J4A_GetStaticMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getNativeOutputSampleRate == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "play";
    sign     = "()V";
    class_J4AC_android_media_AudioTrack.method_play = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_play == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "pause";
    sign     = "()V";
    class_J4AC_android_media_AudioTrack.method_pause = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_pause == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "stop";
    sign     = "()V";
    class_J4AC_android_media_AudioTrack.method_stop = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_stop == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "flush";
    sign     = "()V";
    class_J4AC_android_media_AudioTrack.method_flush = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_flush == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "release";
    sign     = "()V";
    class_J4AC_android_media_AudioTrack.method_release = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_release == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "write";
    sign     = "([BII)I";
    class_J4AC_android_media_AudioTrack.method_write = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_write == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "setStereoVolume";
    sign     = "(FF)I";
    class_J4AC_android_media_AudioTrack.method_setStereoVolume = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_setStereoVolume == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getAudioSessionId";
    sign     = "()I";
    class_J4AC_android_media_AudioTrack.method_getAudioSessionId = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getAudioSessionId == NULL)
        goto fail;

    if (J4A_GetSystemAndroidApiLevel(env) >= 23) {
        class_id = class_J4AC_android_media_AudioTrack.id;
        name     = "getPlaybackParams";
        sign     = "()Landroid/media/PlaybackParams;";
        class_J4AC_android_media_AudioTrack.method_getPlaybackParams = J4A_GetMethodID__catchAll(env, class_id, name, sign);
        if (class_J4AC_android_media_AudioTrack.method_getPlaybackParams == NULL)
            goto fail;
    }

    if (J4A_GetSystemAndroidApiLevel(env) >= 23) {
        class_id = class_J4AC_android_media_AudioTrack.id;
        name     = "setPlaybackParams";
        sign     = "(Landroid/media/PlaybackParams;)V";
        class_J4AC_android_media_AudioTrack.method_setPlaybackParams = J4A_GetMethodID__catchAll(env, class_id, name, sign);
        if (class_J4AC_android_media_AudioTrack.method_setPlaybackParams == NULL)
            goto fail;
    }

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getStreamType";
    sign     = "()I";
    class_J4AC_android_media_AudioTrack.method_getStreamType = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getStreamType == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getSampleRate";
    sign     = "()I";
    class_J4AC_android_media_AudioTrack.method_getSampleRate = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getSampleRate == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "getPlaybackRate";
    sign     = "()I";
    class_J4AC_android_media_AudioTrack.method_getPlaybackRate = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_getPlaybackRate == NULL)
        goto fail;

    class_id = class_J4AC_android_media_AudioTrack.id;
    name     = "setPlaybackRate";
    sign     = "(I)I";
    class_J4AC_android_media_AudioTrack.method_setPlaybackRate = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_AudioTrack.method_setPlaybackRate == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "android.media.AudioTrack");
    ret = 0;
    fail:
    return ret;
}



