//
// Created by Administrator on 2016/12/5.
//

#ifndef YTXPLAYER_MESSAGEQUEUE_H
#define YTXPLAYER_MESSAGEQUEUE_H

#include <pthread.h>
#include "ffinc.h"

typedef struct AVMessage {
    int what;
    int arg1;
    int arg2;
    void *priv_data;
    struct AVMessage *next;
} AVMessage;


typedef struct VMessageData{

    image_t *img;
    char *y;
    char *u;
    char *v;
    int videoWidth;
    int videoHeight;

}VMessageData;


class MessageQueue{
private:
    AVMessage*		mFirst;
    AVMessage*		mLast;
    int					mNbMessages;
    int					mSize;
    bool				mAbortRequest;
    pthread_mutex_t     mLock;
    pthread_cond_t		mCondition;

    AVMessage *mRecycleMsg;
    int mRecycleCount;
    int mAllocCount;

public:
    MessageQueue();
    ~MessageQueue();

    void flush();
    int put(AVMessage* msg);

    /* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
    int get(AVMessage *msg, bool block);

    int size();

    void abort();

};

#endif //YTXPLAYER_MESSAGEQUEUE_H
