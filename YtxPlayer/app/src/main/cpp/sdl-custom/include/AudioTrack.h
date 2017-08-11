//
// Created by Administrator on 2017/7/18.
//

#ifndef YTXPLAYER_AUDIOTRACK_H
#define YTXPLAYER_AUDIOTRACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "J4aBase.h"

jobject J4AC_android_media_AudioTrack__AudioTrack(JNIEnv *env, jint streamType, jint sampleRateInHz,
                                                  jint channelConfig, jint audioFormat,
                                                  jint bufferSizeInBytes, jint mode);

jobject J4AC_android_media_AudioTrack__AudioTrack__catchAll(JNIEnv *env, jint streamType,
                                                            jint sampleRateInHz, jint channelConfig,
                                                            jint audioFormat,
                                                            jint bufferSizeInBytes, jint mode);

jobject
J4AC_android_media_AudioTrack__AudioTrack__asGlobalRef__catchAll(JNIEnv *env, jint streamType,
                                                                 jint sampleRateInHz,
                                                                 jint channelConfig,
                                                                 jint audioFormat,
                                                                 jint bufferSizeInBytes, jint mode);

jint J4AC_android_media_AudioTrack__getMinBufferSize(JNIEnv *env, jint sampleRateInHz,
                                                     jint channelConfig, jint audioFormat);

jint J4AC_android_media_AudioTrack__getMinBufferSize__catchAll(JNIEnv *env, jint sampleRateInHz,
                                                               jint channelConfig,
                                                               jint audioFormat);

jfloat J4AC_android_media_AudioTrack__getMaxVolume(JNIEnv *env);

jfloat J4AC_android_media_AudioTrack__getMaxVolume__catchAll(JNIEnv *env);

jfloat J4AC_android_media_AudioTrack__getMinVolume(JNIEnv *env);

jfloat J4AC_android_media_AudioTrack__getMinVolume__catchAll(JNIEnv *env);

jint J4AC_android_media_AudioTrack__getNativeOutputSampleRate(JNIEnv *env, jint streamType);

jint
J4AC_android_media_AudioTrack__getNativeOutputSampleRate__catchAll(JNIEnv *env, jint streamType);

void J4AC_android_media_AudioTrack__play(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__play__catchAll(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__pause(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__pause__catchAll(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__stop(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__stop__catchAll(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__flush(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__flush__catchAll(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__release(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__release__catchAll(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__write(JNIEnv *env, jobject thiz, jbyteArray audioData,
                                          jint offsetInBytes, jint sizeInBytes);

jint J4AC_android_media_AudioTrack__write__catchAll(JNIEnv *env, jobject thiz, jbyteArray audioData,
                                                    jint offsetInBytes, jint sizeInBytes);

jint J4AC_android_media_AudioTrack__setStereoVolume(JNIEnv *env, jobject thiz, jfloat leftGain,
                                                    jfloat rightGain);

jint
J4AC_android_media_AudioTrack__setStereoVolume__catchAll(JNIEnv *env, jobject thiz, jfloat leftGain,
                                                         jfloat rightGain);

jint J4AC_android_media_AudioTrack__getAudioSessionId(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getAudioSessionId__catchAll(JNIEnv *env, jobject thiz);

jobject J4AC_android_media_AudioTrack__getPlaybackParams(JNIEnv *env, jobject thiz);

jobject J4AC_android_media_AudioTrack__getPlaybackParams__catchAll(JNIEnv *env, jobject thiz);

jobject
J4AC_android_media_AudioTrack__getPlaybackParams__asGlobalRef__catchAll(JNIEnv *env, jobject thiz);

void J4AC_android_media_AudioTrack__setPlaybackParams(JNIEnv *env, jobject thiz, jobject params);

void J4AC_android_media_AudioTrack__setPlaybackParams__catchAll(JNIEnv *env, jobject thiz,
                                                                jobject params);

jint J4AC_android_media_AudioTrack__getStreamType(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getStreamType__catchAll(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getSampleRate(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getSampleRate__catchAll(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getPlaybackRate(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__getPlaybackRate__catchAll(JNIEnv *env, jobject thiz);

jint J4AC_android_media_AudioTrack__setPlaybackRate(JNIEnv *env, jobject thiz, jint sampleRateInHz);

jint J4AC_android_media_AudioTrack__setPlaybackRate__catchAll(JNIEnv *env, jobject thiz,
                                                              jint sampleRateInHz);

int J4A_loadClass__J4AC_android_media_AudioTrack(JNIEnv *env);

#define J4A_HAVE_SIMPLE__J4AC_android_media_AudioTrack

#define J4AC_AudioTrack__AudioTrack J4AC_android_media_AudioTrack__AudioTrack
#define J4AC_AudioTrack__AudioTrack__asGlobalRef__catchAll J4AC_android_media_AudioTrack__AudioTrack__asGlobalRef__catchAll
#define J4AC_AudioTrack__AudioTrack__catchAll J4AC_android_media_AudioTrack__AudioTrack__catchAll
#define J4AC_AudioTrack__getMinBufferSize J4AC_android_media_AudioTrack__getMinBufferSize
#define J4AC_AudioTrack__getMinBufferSize__catchAll J4AC_android_media_AudioTrack__getMinBufferSize__catchAll
#define J4AC_AudioTrack__getMaxVolume J4AC_android_media_AudioTrack__getMaxVolume
#define J4AC_AudioTrack__getMaxVolume__catchAll J4AC_android_media_AudioTrack__getMaxVolume__catchAll
#define J4AC_AudioTrack__getMinVolume J4AC_android_media_AudioTrack__getMinVolume
#define J4AC_AudioTrack__getMinVolume__catchAll J4AC_android_media_AudioTrack__getMinVolume__catchAll
#define J4AC_AudioTrack__getNativeOutputSampleRate J4AC_android_media_AudioTrack__getNativeOutputSampleRate
#define J4AC_AudioTrack__getNativeOutputSampleRate__catchAll J4AC_android_media_AudioTrack__getNativeOutputSampleRate__catchAll
#define J4AC_AudioTrack__play J4AC_android_media_AudioTrack__play
#define J4AC_AudioTrack__play__catchAll J4AC_android_media_AudioTrack__play__catchAll
#define J4AC_AudioTrack__pause J4AC_android_media_AudioTrack__pause
#define J4AC_AudioTrack__pause__catchAll J4AC_android_media_AudioTrack__pause__catchAll
#define J4AC_AudioTrack__stop J4AC_android_media_AudioTrack__stop
#define J4AC_AudioTrack__stop__catchAll J4AC_android_media_AudioTrack__stop__catchAll
#define J4AC_AudioTrack__flush J4AC_android_media_AudioTrack__flush
#define J4AC_AudioTrack__flush__catchAll J4AC_android_media_AudioTrack__flush__catchAll
#define J4AC_AudioTrack__release J4AC_android_media_AudioTrack__release
#define J4AC_AudioTrack__release__catchAll J4AC_android_media_AudioTrack__release__catchAll
#define J4AC_AudioTrack__write J4AC_android_media_AudioTrack__write
#define J4AC_AudioTrack__write__catchAll J4AC_android_media_AudioTrack__write__catchAll
#define J4AC_AudioTrack__setStereoVolume J4AC_android_media_AudioTrack__setStereoVolume
#define J4AC_AudioTrack__setStereoVolume__catchAll J4AC_android_media_AudioTrack__setStereoVolume__catchAll
#define J4AC_AudioTrack__getAudioSessionId J4AC_android_media_AudioTrack__getAudioSessionId
#define J4AC_AudioTrack__getAudioSessionId__catchAll J4AC_android_media_AudioTrack__getAudioSessionId__catchAll
#define J4AC_AudioTrack__getPlaybackParams J4AC_android_media_AudioTrack__getPlaybackParams
#define J4AC_AudioTrack__getPlaybackParams__asGlobalRef__catchAll J4AC_android_media_AudioTrack__getPlaybackParams__asGlobalRef__catchAll
#define J4AC_AudioTrack__getPlaybackParams__catchAll J4AC_android_media_AudioTrack__getPlaybackParams__catchAll
#define J4AC_AudioTrack__setPlaybackParams J4AC_android_media_AudioTrack__setPlaybackParams
#define J4AC_AudioTrack__setPlaybackParams__catchAll J4AC_android_media_AudioTrack__setPlaybackParams__catchAll
#define J4AC_AudioTrack__getStreamType J4AC_android_media_AudioTrack__getStreamType
#define J4AC_AudioTrack__getStreamType__catchAll J4AC_android_media_AudioTrack__getStreamType__catchAll
#define J4AC_AudioTrack__getSampleRate J4AC_android_media_AudioTrack__getSampleRate
#define J4AC_AudioTrack__getSampleRate__catchAll J4AC_android_media_AudioTrack__getSampleRate__catchAll
#define J4AC_AudioTrack__getPlaybackRate J4AC_android_media_AudioTrack__getPlaybackRate
#define J4AC_AudioTrack__getPlaybackRate__catchAll J4AC_android_media_AudioTrack__getPlaybackRate__catchAll
#define J4AC_AudioTrack__setPlaybackRate J4AC_android_media_AudioTrack__setPlaybackRate
#define J4AC_AudioTrack__setPlaybackRate__catchAll J4AC_android_media_AudioTrack__setPlaybackRate__catchAll
#define J4A_loadClass__J4AC_AudioTrack J4A_loadClass__J4AC_android_media_AudioTrack

#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_AUDIOTRACK_H