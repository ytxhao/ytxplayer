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
/*
libLoader.loadLibrary("gnustl_shared");
libLoader.loadLibrary("mp4v2");
libLoader.loadLibrary("faad");
libLoader.loadLibrary("faac");
libLoader.loadLibrary("rtmp");
libLoader.loadLibrary("x264");
libLoader.loadLibrary("avutil");
libLoader.loadLibrary("swresample");
libLoader.loadLibrary("swscale");
libLoader.loadLibrary("postproc");
libLoader.loadLibrary("avcodec");
libLoader.loadLibrary("avformat");
libLoader.loadLibrary("avdevice");
libLoader.loadLibrary("avfilter");
*/
#define __STDC_CONSTANT_MACROS

#include "ALog-priv.h"

#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

jstring  Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */);

#ifdef __cplusplus
}
#endif


#endif //YTXPLAYER_YTX_MEDIA_PLAYER_H
