//
// Created by Administrator on 2016/10/13.
//

#include "ytxplayer/frame_queue_video.h"
#define TAG "YTX-FrameQueueVideo-JNI"
#include "ytxplayer/ALog-priv.h"


FrameQueueVideo::~FrameQueueVideo(){
    frameQueueDestroy();
}

void FrameQueueVideo::frameQueueUnrefItem(Frame *vp) {
    int i;
    for (i = 0; i < 3; i++) {
        if (vp->out_buffer_video_yuv[i] != NULL) {
            free(vp->out_buffer_video_yuv[i]);
            vp->out_buffer_video_yuv[i] = NULL;
        }
    }

}


int FrameQueueVideo::frameQueueInit(int max_size, int keep_last) {
    int i = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    this->max_size = max_size;
    this->keep_last = keep_last;
    for (i = 0; i < max_size; i++) {
        queue[i].out_buffer_video_yuv[0] = NULL;
        queue[i].out_buffer_video_yuv[1] = NULL;
        queue[i].out_buffer_video_yuv[2] = NULL;
    }
    return 1;
}

void FrameQueueVideo::frameQueueDestroy() {
    int i;
    for(i=0;i<max_size;i++){
        Frame *vp = &queue[i];
        frameQueueUnrefItem(vp);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}