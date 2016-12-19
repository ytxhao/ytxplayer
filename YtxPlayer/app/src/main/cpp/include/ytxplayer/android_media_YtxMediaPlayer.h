//
// Created by Administrator on 2016/9/2.
//

#ifndef YTXPLAYER_YTX_MEDIA_PLAYER_H
#define YTXPLAYER_YTX_MEDIA_PLAYER_H

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


jstring  Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */);



/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    _setGlSurface
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL android_media_player_setGlSurface
        (JNIEnv *, jobject, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    died
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_died
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    disconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_disconnect
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setDataSource
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL android_media_player_setDataSource
        (JNIEnv *, jobject, jstring);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setDataSource
 * Signature: (IJJ)I
 */
//JNIEXPORT jint JNICALL android_media_player_setDataSource
//        (JNIEnv *, jobject, jint, jlong, jlong);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    prepare
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepare
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    prepareAsync
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepareAsync
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_start
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_stop
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    release
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_release
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    pause
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_pause
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    isPlaying
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_isPlaying
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getVideoWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoWidth
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getVideoHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoHeight
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    seekTo
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_seekTo
        (JNIEnv *, jobject, jint);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getCurrentPosition
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getCurrentPosition
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getDuration
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getDuration
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    reset
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_reset
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setLooping
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_setLooping
        (JNIEnv *, jobject, jint);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    isLooping
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_isLooping
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setVolume
 * Signature: (FF)I
 */
JNIEXPORT jint JNICALL android_media_player_setVolume
        (JNIEnv *, jobject, jfloat, jfloat);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setAudioSessionId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_setAudioSessionId
        (JNIEnv *, jobject, jint);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getAudioSessionId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getAudioSessionId
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setAuxEffectSendLevel
 * Signature: (F)I
 */
JNIEXPORT jint JNICALL android_media_player_setAuxEffectSendLevel
        (JNIEnv *, jobject, jfloat);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    attachAuxEffect
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_attachAuxEffect
        (JNIEnv *, jobject, jint);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    setRetransmitEndpoint
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL android_media_player_setRetransmitEndpoint
        (JNIEnv *, jobject, jstring, jlong);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    updateProxyConfig
 * Signature: (Ljava/lang/String;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL android_media_player_updateProxyConfig
        (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    clear_l
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_clear_l
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    seekTo_l
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_seekTo_l
        (JNIEnv *, jobject, jint);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    prepareAsync_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepareAsync_l
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    getDuration_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getDuration_l
        (JNIEnv *, jobject);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    reset_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_reset_l
        (JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif


#endif //YTXPLAYER_YTX_MEDIA_PLAYER_H
