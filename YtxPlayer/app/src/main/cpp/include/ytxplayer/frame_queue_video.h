//
// Created by Administrator on 2016/10/13.
//

#ifndef YTXPLAYER_FRAME_QUEUE_VIDEO_H
#define YTXPLAYER_FRAME_QUEUE_VIDEO_H


#include <pthread.h>
#include "ffinc.h"



class FrameQueueVideo{
public:

    ~FrameQueueVideo();
    void frameQueueUnrefItem(Frame *vp);
    int frameQueueInit(int max_size,int keep_last);
    void frameQueueDestroy();
    void frameQueueSignal();
    Frame* frameQueuePeek();
    Frame* frameQueuePeekNext();
    Frame* frameQueuePeekLast();
    Frame* frameQueuePeekWritable();
    Frame* frameQueuePeekReadable();
    void frameQueuePush();
    void frameQueueNext();
    void frameQueueReset();
    /**
     * return the number of undisplayed frames in the queue
     */
    int frameQueueNumRemaining();
    /**
     * return last shown position
     */
    int64_t frameQueueLastPos();

    int size=0;

    Frame queue[FRAME_QUEUE_SIZE];
    //必须手动初始化否则是随机数据
    int rindex=0;  //read index
    int windex=0;   //write index

    int max_size=0;
    int keep_last =0 ;
    int rindex_shown = 0 ;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int tmp=0;

};




#endif //YTXPLAYER_FRAME_QUEUE_H
