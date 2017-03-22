//
// Created by Administrator on 2016/10/13.
//
#define TAG "YTX-FrameQueue-JNI"

#include "ytxplayer/ALog-priv.h"

#include "ytxplayer/frame_queue.h"


void FrameQueue::frameQueueReset() {
    pthread_mutex_lock(&mutex);
    rindex = 0;
    windex = 0;
    rindex_shown = 1;
    size = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void FrameQueue::frameQueueSignal() {
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

Frame *FrameQueue::frameQueuePeek() {
    return &queue[(rindex + rindex_shown) % max_size];
}


Frame *FrameQueue::frameQueuePeekNext() {
    return &queue[(rindex + rindex_shown + 1) % max_size];
}

Frame *FrameQueue::frameQueuePeekLast() {
    return &queue[rindex];
}

Frame *FrameQueue::frameQueuePeekWritable() {
    pthread_mutex_lock(&mutex);
    while (size >= max_size) {
        pthread_cond_wait(&cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    return &queue[windex];
}

Frame *FrameQueue::frameQueuePeekReadable() {

    pthread_mutex_lock(&mutex);
    while (size - rindex_shown <= 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    return &queue[(rindex + rindex_shown) % max_size];
}

void FrameQueue::frameQueuePush() {

    if (++windex == max_size) {
        windex = 0;
    }
    pthread_mutex_lock(&mutex);
    size++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

}

void FrameQueue::frameQueueNext() {
    if (keep_last && !rindex_shown) {
        rindex_shown = 1;
        return;
    }

    frameQueueUnrefItem(&queue[rindex]);
    if (++rindex == max_size) {
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
    if (rindex_shown) {
        return fp->pos;
    } else {
        return -1;
    }

}