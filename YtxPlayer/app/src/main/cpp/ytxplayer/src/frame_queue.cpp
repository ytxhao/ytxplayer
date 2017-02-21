//
// Created by Administrator on 2016/10/13.
//

#include "ytxplayer/frame_queue.h"
#define TAG "YTX-FrameQueue-JNI"
#include "ytxplayer/ALog-priv.h"



//FrameQueue::~FrameQueue(){
//    frameQueueDestroy();
//}

/*
void FrameQueue::frameQueueUnrefItem(Frame *vp) {
    int i;
    //释放subtitle空间
//    for(i=0;i<vp->sub.num_rects;i++){
//        av_freep(&vp->subrects[i]->data[0]);
//        av_freep(&vp->subrects[i]);
//    }
//    av_freep(&vp->subrects);
    av_frame_unref(vp->frame);
  //  avsubtitle_free(&vp->sub);

 //   free(vp->out_buffer_audio);

}
*/
void FrameQueue::frameQueueReset(){
    pthread_mutex_lock(&mutex);
    rindex = 0;
    windex = 0;
    rindex_shown = 1;
    size = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

/*
int FrameQueue::frameQueueInit(int max_size, int keep_last) {
    int i = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    this->max_size = max_size;
    this->keep_last = keep_last;
    for (i = 0; i < max_size; i++) {
        queue[i].frame = av_frame_alloc();
        if (queue[i].frame == NULL) {
            return AVERROR(ENOMEM);
        }
    }
    return 1;
}

void FrameQueue::frameQueueDestroy() {
    int i;
    for(i=0;i<max_size;i++){
        Frame *vp = &queue[i];
        frameQueueUnrefItem(vp);
        av_frame_free(&vp->frame);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
*/
void FrameQueue::frameQueueSignal() {
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

Frame* FrameQueue::frameQueuePeek() {
    return &queue[(rindex+rindex_shown)%max_size];
}


Frame* FrameQueue::frameQueuePeekNext() {
    return &queue[(rindex+rindex_shown+1)%max_size];
}

Frame* FrameQueue::frameQueuePeekLast() {
    return &queue[rindex];
}

Frame* FrameQueue::frameQueuePeekWritable() {
    pthread_mutex_lock(&mutex);
    ALOGI("frameQueuePeekWritable size=%d\n",size);
    while(size >= max_size){
        pthread_cond_wait(&cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    ALOGI("frameQueuePeekWritable windex=%d\n",windex);
    return &queue[windex];
}

Frame *FrameQueue::frameQueuePeekReadable() {

    pthread_mutex_lock(&mutex);
    while (size - rindex_shown <= 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    ALOGI("frameQueuePeekReadable rindex=%d\n",rindex);
    return &queue[(rindex + rindex_shown) % max_size];
}

void FrameQueue::frameQueuePush() {

    if(++windex == max_size){
        windex = 0;
    }
    pthread_mutex_lock(&mutex);
    size++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

}

void FrameQueue::frameQueueNext() {
    if(keep_last && !rindex_shown){
        rindex_shown = 1;
        return;
    }

    frameQueueUnrefItem(&queue[rindex]);
    if(++rindex == max_size){
        rindex = 0;
    }
    pthread_mutex_lock(&mutex);
    size--;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

int FrameQueue::frameQueueNumRemaining() {
    return size - rindex_shown;
}

int64_t FrameQueue::frameQueueLastPos() {
    Frame *fp = &queue[rindex];
    if(rindex_shown){
        return fp->pos;
    }else{
        return -1;
    }

}