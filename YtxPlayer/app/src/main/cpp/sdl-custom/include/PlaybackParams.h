//
// Created by Administrator on 2017/8/7.
//

#ifndef YTXPLAYER_PLAYBACKPARAMS_H
#define YTXPLAYER_PLAYBACKPARAMS_H
#include "J4aBase.h"

jobject J4AC_android_media_PlaybackParams__setSpeed(JNIEnv *env, jobject thiz, jfloat speed);
jobject J4AC_android_media_PlaybackParams__setSpeed__catchAll(JNIEnv *env, jobject thiz, jfloat speed);
jobject J4AC_android_media_PlaybackParams__setSpeed__asGlobalRef__catchAll(JNIEnv *env, jobject thiz, jfloat speed);
int J4A_loadClass__J4AC_android_media_PlaybackParams(JNIEnv *env);

#define J4A_HAVE_SIMPLE__J4AC_android_media_PlaybackParams

#define J4AC_PlaybackParams__setSpeed J4AC_android_media_PlaybackParams__setSpeed
#define J4AC_PlaybackParams__setSpeed__asGlobalRef__catchAll J4AC_android_media_PlaybackParams__setSpeed__asGlobalRef__catchAll
#define J4AC_PlaybackParams__setSpeed__catchAll J4AC_android_media_PlaybackParams__setSpeed__catchAll
#define J4A_loadClass__J4AC_PlaybackParams J4A_loadClass__J4AC_android_media_PlaybackParams

#endif //YTXPLAYER_PLAYBACKPARAMS_H
