//
// Created by Administrator on 2016/12/1.
//

#ifndef YTXPLAYER_VIDEOSTATEINFO_H
#define YTXPLAYER_VIDEOSTATEINFO_H

#include "InputStream.h"
#include "ffinc.h"
#include "frame_queue.h"

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

class VideoStateInfo{
public:
    VideoStateInfo();
    ~VideoStateInfo();
    AVFormatContext *pFormatCtx;

    InputStream *streamVideo;
    InputStream *streamAudio;

    FrameQueue *frameQueueVideo;
    FrameQueue *frameQueueAudio;
    int *mCurrentState;

};
#endif //YTXPLAYER_VIDEOSTATEINFO_H
