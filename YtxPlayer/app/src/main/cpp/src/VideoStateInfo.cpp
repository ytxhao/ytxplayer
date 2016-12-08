//
// Created by Administrator on 2016/12/1.
//
#include "VideoStateInfo.h"

VideoStateInfo::VideoStateInfo(){
    frameQueueVideo = new FrameQueue();
    frameQueueAudio = new FrameQueue();
    streamVideo = new InputStream();
    streamAudio = new InputStream();
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);

}

VideoStateInfo::~VideoStateInfo() {
    delete frameQueueVideo;
    delete frameQueueAudio;
    delete streamVideo;
    delete streamAudio;
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

void VideoStateInfo::notify() {
    pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}


void VideoStateInfo::notifyAll() {
    pthread_mutex_lock(&mLock);
    pthread_cond_broadcast(&mCondition);
    pthread_mutex_unlock(&mLock);
}

void VideoStateInfo::waitOnNotify(int mCurrentState)
{
    pthread_mutex_lock(&mLock);
    while (*this->mCurrentState == mCurrentState){
        pthread_cond_wait(&mCondition, &mLock);
    }

    pthread_mutex_unlock(&mLock);
}
