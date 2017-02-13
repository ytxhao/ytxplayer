//
// Created by Administrator on 2016/10/13.
//

#include "ytxplayer/frame_queue_subtitle.h"
#define TAG "YTX-FrameQueueSubtitle-JNI"
#include "ALog-priv.h"


FrameQueueSubtitle::~FrameQueueSubtitle(){
    frameQueueDestroy();
}

void FrameQueueSubtitle::frameQueueUnrefItem(Frame *vp) {

    if(vp->out_buffer_audio != NULL){
        free(vp->out_buffer_audio);
    }


}

int FrameQueueSubtitle::frameQueueInit(int max_size, int keep_last) {
    int i = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    this->max_size = max_size;
    this->keep_last = keep_last;
    for (i = 0; i < max_size; i++) {
        queue[i].out_buffer_audio = NULL;
        queue[i].out_buffer_audio_size = 0;
    }

    return 1;
}

void FrameQueueSubtitle::frameQueueDestroy() {
    int i;
    for(i=0;i<max_size;i++){
        Frame *vp = &queue[i];
        frameQueueUnrefItem(vp);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}