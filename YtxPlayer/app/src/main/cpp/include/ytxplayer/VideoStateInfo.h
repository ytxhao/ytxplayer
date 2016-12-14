//
// Created by Administrator on 2016/12/1.
//

#ifndef YTXPLAYER_VIDEOSTATEINFO_H
#define YTXPLAYER_VIDEOSTATEINFO_H

#include "InputStream.h"
#include "ffinc.h"
#include "frame_queue.h"
#include "avPacketList.h"
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

    AVFormatContext *pFormatCtx;

    InputStream *streamVideo;
    InputStream *streamAudio;

    FrameQueue *frameQueueVideo;
    FrameQueue *frameQueueAudio;
    int *mCurrentState;

    volatile bool seekReq;
    int seekFlags;
    int64_t seekPos;
    int64_t seekRel;
    MAVPacket *flushPkt;
    Clock *vidClk;
    Clock *extClk;

    pthread_mutex_t     		mLock;
    pthread_cond_t				mCondition;

    pthread_mutex_t wait_mutex;
    pthread_cond_t continue_read_thread;

    int eof;

    int st_index[AVMEDIA_TYPE_NB];

    int currentTime;

};
#endif //YTXPLAYER_VIDEOSTATEINFO_H
