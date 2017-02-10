//
// Created by Administrator on 2016/10/13.
//

#ifndef YTXPLAYER_FRAME_QUEUE_VIDEO_H
#define YTXPLAYER_FRAME_QUEUE_VIDEO_H


#include <pthread.h>
#include "ffinc.h"
#include "frame_queue.h"



class FrameQueueVideo : public FrameQueue{
public:
    virtual ~FrameQueueVideo();
    void frameQueueUnrefItem(Frame *vp);
    int frameQueueInit(int max_size,int keep_last);
    void frameQueueDestroy();

};




#endif //YTXPLAYER_FRAME_QUEUE_H
