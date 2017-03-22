//
// Created by Administrator on 2016/9/7.
//
#define TAG "FFMpegThread"

#include "ytxplayer/Thread.h"
#include "ytxplayer/ALog-priv.h"


Thread::Thread() {
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
}

Thread::~Thread() {
}

void Thread::start() {
    handleRun(NULL);
}

void Thread::startAsync() {
    pthread_create(&mThread, NULL, startThread, this);
}

int Thread::wait() {
    return pthread_join(mThread, NULL);
}

void Thread::stop() {
}

void *Thread::startThread(void *ptr) {
    ALOGI("starting thread\n");
    Thread *thread = (Thread *) ptr;
    thread->mRunning = true;
    thread->handleRun(ptr);
    thread->mRunning = false;
    ALOGI("thread ended\n");
    pthread_exit(NULL);
}

void Thread::waitOnNotify() {
    pthread_mutex_lock(&mLock);
    pthread_cond_wait(&mCondition, &mLock);
    pthread_mutex_unlock(&mLock);
}

void Thread::notify() {
    pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}

void Thread::handleRun(void *ptr) {
}
