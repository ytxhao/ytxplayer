//
// Created by Administrator on 2016/10/13.
//

#ifndef YTXPLAYER_FRAME_QUEUE_SUBTITLE_H
#define YTXPLAYER_FRAME_QUEUE_SUBTITLE_H


#include <pthread.h>
#include "ffinc.h"
#include "frame_queue.h"
class FrameQueueSubtitle : public FrameQueue{
public:
    virtual ~FrameQueueSubtitle();
    void frameQueueUnrefItem(Frame *vp);
    int frameQueueInit(int max_size,int keep_last);
    void frameQueueDestroy();
};




#endif //YTXPLAYER_FRAME_QUEUE_H
