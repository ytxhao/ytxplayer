//
// Created by Administrator on 2016/12/1.
//

#ifndef YTXPLAYER_VIDEOSTATEINFO_H
#define YTXPLAYER_VIDEOSTATEINFO_H

#include "InputStream.h"
#include "ffinc.h"
#include "frame_queue.h"
class VideoStateInfo{
public:
    VideoStateInfo();
    ~VideoStateInfo();
    AVFormatContext *pFormatCtx;

    InputStream *streamVideo;
    InputStream *streamAudio;

    FrameQueue *frameQueueVideo;
    FrameQueue *frameQueueAudio;

};
#endif //YTXPLAYER_VIDEOSTATEINFO_H
