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

#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "libswresample/swresample.h"
#include <libavutil/opt.h>
#include <libavutil/time.h>

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control */
#define SDL_VOLUME_STEP (SDL_MIX_MAXVOLUME / 50)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define MIN_FRAMES 25


#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9

#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

#define MAX_AUDIO_FRAME_SIZE 48000 * 4

static unsigned sws_flags = SWS_BICUBIC;


typedef struct Frame {
    AVFrame *frame;
  //  AVFrame *frameYuv;
    unsigned char *out_buffer_audio;
    int out_buffer_audio_size;
    AVSubtitle sub;
    AVSubtitleRect **subrects;  /* rescaled subtitle rectangles in yuva */
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int allocated;
    int reallocate;
    int width;
    int height;
    AVRational sar;
} Frame;


#ifdef __cplusplus
}
#endif

#endif //YTXPLAYER_FFINC_H
