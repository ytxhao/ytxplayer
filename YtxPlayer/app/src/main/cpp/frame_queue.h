//
// Created by Administrator on 2016/10/13.
//

#ifndef YTXPLAYER_FRAME_QUEUE_H
#define YTXPLAYER_FRAME_QUEUE_H


#include <pthread.h>
#include "ffinc.h"


#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct Frame {
    AVFrame *frame;
    AVFrame *frameYuv;
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


class FrameQueue{
public:

    void frameQueueUnrefItem(Frame *vp);
    int frameQueueInit(int max_size,int keep_last,InputStream*  mStream);
    void frameQueueDestory();
    void frameQueueSignal();
    Frame* frameQueuePeek();
    Frame* frameQueuePeekNext();
    Frame* frameQueuePeekLast();
    Frame* frameQueuePeekWritable();
    Frame* frameQueuePeekReadable();
    void frameQueuePush();
    void frameQueueNext();
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
    int rindex=0;
    int windex=0;

    int max_size=0;
    int keep_last =0 ;
    int rindex_shown = 0 ;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    InputStream*                mStream;

};




#endif //YTXPLAYER_FRAME_QUEUE_H
