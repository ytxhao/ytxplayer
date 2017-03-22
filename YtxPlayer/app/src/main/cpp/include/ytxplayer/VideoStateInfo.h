//
// Created by Administrator on 2016/12/1.
//

#ifndef YTXPLAYER_VIDEOSTATEINFO_H
#define YTXPLAYER_VIDEOSTATEINFO_H

#include "InputStream.h"
#include "ffinc.h"
#include "frame_queue.h"
#include "frame_queue_video.h"
#include "frame_queue_audio.h"
#include "frame_queue_subtitle.h"
#include "avPacketList.h"
#include "MessageQueue.h"
#include <ytxplayer/MessageLoop.h>
#include "jni.h"
enum media_player_states {
    MEDIA_PLAYER_STATE_ERROR        = 0,
    MEDIA_PLAYER_IDLE               = 1 << 0,
    MEDIA_PLAYER_INITIALIZED        = 1 << 1,
    MEDIA_PLAYER_PREPARING          = 1 << 2,
    MEDIA_PLAYER_PREPARED           = 1 << 3,
    MEDIA_PLAYER_DECODED            = 1 << 4,
    MEDIA_PLAYER_STARTED            = 1 << 5,
    MEDIA_PLAYER_PAUSED             = 1 << 6,
    MEDIA_PLAYER_STOPPED            = 1 << 7,
    MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 8
};

typedef struct Clock {
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;


class VideoStateInfo{
public:
    VideoStateInfo();
    ~VideoStateInfo();
    char* join(char *s1, char *s2);
    void notify();
    void notifyAll();
    void waitOnNotify(int mCurrentState);

    double getClock(Clock *c);
    void setClock(Clock *c, double pts, int serial);
    void setClockAt(Clock *c, double pts, int serial, double time);
    void setClockSpeed(Clock *c, double speed);
    void initClock(Clock *c, int *queue_serial);
    void updateVideoPts(double pts, int64_t pos, int serial);
    void syncClock2Slave(Clock *c, Clock *slave);

    void init_opts(void);
    void uninit_opts(void);
public:
    AVFormatContext *pFormatCtx;

    AVFilterContext *in_video_filter;   // the first filter in the video chain
    AVFilterContext *out_video_filter;  // the last filter in the video chain
    AVDictionary *sws_dict;
    InputStream *streamVideo;
    InputStream *streamAudio;
    InputStream *streamSubtitle;

    FrameQueue *frameQueueVideo;
    FrameQueue *frameQueueAudio;
    FrameQueue *frameQueueSubtitle;
    MessageLoop   *mMessageLoop;
    int *mCurrentState;

    volatile bool seekReq;
    int seekFlags;
    int64_t seekPos;
    int64_t seekRel;
    MAVPacket *flushPkt;
    Clock *audClk;
    Clock *vidClk;
    Clock *extClk;

    pthread_mutex_t     		mLock;
    pthread_cond_t				mCondition;

    pthread_mutex_t wait_mutex;
    pthread_cond_t continue_read_thread;

    int eof;

    int st_index[AVMEDIA_TYPE_NB]={0};

    int currentTime;

    unsigned char *out_buffer_audio;

    //视频帧率
    AVRational frame_rate_video;


    enum AVSampleFormat in_sample_fmt ;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    int in_sample_rate ;
    //输出采样率
    int out_sample_rate = 44100;
    int out_channel_nb;
    int out_nb_samples;
    uint64_t  in_ch_layout;
    uint64_t out_ch_layout;

   // SwrContext *swrCtx;

    volatile bool   isFirstAudioFrame = true;

    MessageQueue *messageQueueAudio;
    MessageQueue *messageQueueVideo;

    int pkt_serial_audio = 0;
    int pkt_serial_video = 0;
    int pkt_serial_subtitle = 0;
    double max_frame_duration;
//    FILE *fp_yuv;
//    FILE *fp_pcm;
//    FILE *fp_pcm1;
//    char file_pcm[100];
//    char file_pcm1[100];
    jobject VideoGlSurfaceViewObj;
    jobject GraphicRendererObj;
    char *mStorageDir;

    int mVideoWidth =0;
    int mVideoHeight = 0;



    int vfilter_idx = 0;


    const char **vfilters_list = NULL;
    int nb_vfilters = 0;
    char *afilters = NULL;

    double frame_timer;
    double frame_last_returned_time;
    double frame_last_filter_delay;

    int viddec_finished;
    int autorotate = 1;

    int sub_format = -1;
};
#endif //YTXPLAYER_VIDEOSTATEINFO_H
