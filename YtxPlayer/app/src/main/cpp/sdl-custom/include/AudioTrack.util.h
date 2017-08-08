//
// Created by Administrator on 2017/8/7.
//

#ifndef YTXPLAYER_AUDIOTRACK_UTIL_H
#define YTXPLAYER_AUDIOTRACK_UTIL_H

#include "J4aBase.h"
#include "AudioTrack.h"

void J4AC_android_media_AudioTrack__setSpeed(JNIEnv *env, jobject thiz, jfloat speed);
void J4AC_android_media_AudioTrack__setSpeed__catchAll(JNIEnv *env, jobject thiz, jfloat speed);

#ifdef J4A_HAVE_SIMPLE__J4AC_android_media_AudioTrack
#define J4AC_AudioTrack__setSpeed           J4AC_android_media_AudioTrack__setSpeed
#define J4AC_AudioTrack__setSpeed__catchAll J4AC_android_media_AudioTrack__setSpeed__catchAll
#endif

#endif //YTXPLAYER_AUDIOTRACK_UTIL_H
