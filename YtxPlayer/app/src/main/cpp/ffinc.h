//
// Created by Administrator on 2016/9/6.
//

#ifndef YTXPLAYER_FFINC_H
#define YTXPLAYER_FFINC_H
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
//#define __STDC_CONSTANT_MACROS

#include "ALog-priv.h"

#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
typedef struct InputStream {
    AVStream *st;
    AVCodecContext *dec_ctx;
} InputStream;

#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_FFINC_H
