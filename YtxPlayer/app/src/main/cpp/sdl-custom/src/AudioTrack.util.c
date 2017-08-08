//
// Created by Administrator on 2017/8/7.
//

#include "../include/AudioTrack.util.h"

#include "../include/PlaybackParams.h"

#ifndef STREAM_MUSIC
#define STREAM_MUSIC 3
#endif
void J4AC_android_media_AudioTrack__setSpeed(JNIEnv *env, jobject thiz, jfloat speed)
{
    if (J4A_GetSystemAndroidApiLevel(env) < 23) {
        jint sample_rate = J4AC_android_media_AudioTrack__getNativeOutputSampleRate__catchAll(env,
                                                                                              STREAM_MUSIC);
        if (J4A_ExceptionCheck__throwAny(env)) {
            return;
        }
        J4AC_android_media_AudioTrack__setPlaybackRate__catchAll(env, thiz,
                                                                 (jint) (sample_rate * speed));
        return;
    }

    jobject temp = NULL;
    jobject params = J4AC_android_media_AudioTrack__getPlaybackParams(env, thiz);
    if (J4A_ExceptionCheck__throwAny(env) || !params)
        goto fail;

    temp = J4AC_android_media_PlaybackParams__setSpeed(env, params, speed);
    J4A_DeleteLocalRef__p(env, &temp);
    if (J4A_ExceptionCheck__throwAny(env))
        goto fail;

    J4A_ALOGE("%s %f", __func__, (double)speed);
    J4AC_android_media_AudioTrack__setPlaybackParams(env, thiz, params);
    if (J4A_ExceptionCheck__throwAny(env))
        goto fail;

    fail:
    J4A_DeleteLocalRef__p(env, &params);
}

void J4AC_android_media_AudioTrack__setSpeed__catchAll(JNIEnv *env, jobject thiz, jfloat speed)
{
    J4A_ALOGE("%s", __func__);
    J4AC_android_media_AudioTrack__setSpeed(env, thiz, speed);
    if (J4A_ExceptionCheck__catchAll(env))
        return;

    return;
}
