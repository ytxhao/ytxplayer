//
// Created by Administrator on 2016/12/5.
//
#define NDEBUG 0
#define TAG "FFMpegMessageQueue"

#include "ALog-priv.h"

#include "MessageQueue.h"

MessageQueue::MessageQueue() {
    mFirst = NULL;
    mLast = NULL;
    mRecycleMsg = NULL;
    mNbMessages = 0;
    mSize = 0;
    mAbortRequest = false;


    mRecycleCount = 0;
    mAllocCount = 0;
    pthread_mutex_init(&mLock,NULL);
    pthread_cond_init(&mCondition,NULL);
}

MessageQueue::~MessageQueue() {
    flush();
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

int MessageQueue::size() {
    pthread_mutex_lock(&mLock);
    int size = mNbMessages;
    pthread_mutex_unlock(&mLock);
    return size;
}

void MessageQueue::flush() {
    AVMessage *msg, *msg1;

    pthread_mutex_lock(&mLock);

    for(msg = mFirst; msg != NULL; msg = msg1) {
        msg1 = msg->next;
        free(msg);

    }
    mLast = NULL;
    mFirst = NULL;
    mNbMessages = 0;
    mSize = 0;

    pthread_mutex_unlock(&mLock);
}

int MessageQueue::put(AVMessage *msg) {
    AVMessage *msg1;

    msg1 = (AVMessage *) malloc(sizeof(AVMessage));
    if(!msg1){
        return -1;
    }

    *msg1 = *msg;
    msg1->next = NULL;
    pthread_mutex_lock(&mLock);

    if(!mLast){
        mFirst = msg1;
    }else{
        mLast->next = msg1;
    }

    mLast = msg1;
    mNbMessages++;
    mSize += sizeof(*msg1);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
    return 0;

}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int MessageQueue::get(AVMessage *msg, bool block) {

    AVMessage *msg1;
    int ret;

    pthread_mutex_lock(&mLock);
    for(;;){
        if(mAbortRequest){
            ret = -1;
            break;
        }

        msg1 = mFirst;
        if(msg1){
            mFirst = msg1->next;
            if(!mFirst){
                mLast = NULL;
            }

            mNbMessages--;
            mSize -= sizeof(*msg1);
            *msg = *msg1;
            free(msg1);
            ret = 1;
            break;
        }else if(!block){
            ret = 0;
            break;
        }else{
            pthread_cond_wait(&mCondition, &mLock);
        }

    }
    pthread_mutex_unlock(&mLock);
    return ret;

}

void MessageQueue::abort() {


    pthread_mutex_lock(&mLock);
    mAbortRequest = true;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}