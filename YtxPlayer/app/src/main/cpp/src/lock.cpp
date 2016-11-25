//
// Created by Administrator on 2016/11/18.
//

#include <pthread.h>
#include "Lock.h"

Lock::Lock() {
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
}

Lock::~Lock() {
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

void Lock::lock() {
    pthread_mutex_lock(&mLock);
}

void Lock::unlock() {
    pthread_mutex_unlock(&mLock);
}

void Lock::condWait() {
    pthread_cond_wait(&mCondition, &mLock);
}

void Lock::condSignal() {
    pthread_cond_signal(&mCondition);
}