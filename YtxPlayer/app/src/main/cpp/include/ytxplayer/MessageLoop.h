//
// Created by Administrator on 2016/12/6.
//

#ifndef YTXPLAYER_MESSAGELOOP_H
#define YTXPLAYER_MESSAGELOOP_H

#include "Thread.h"
#include "MessageQueue.h"
#include "MediaPlayerListener.h"

class MessageLoop : public Thread{


public:
    MessageLoop();
    ~MessageLoop();

    MessageQueue *mQueue;
    MediaPlayerListener* listener;
    void                setMsgListener(MediaPlayerListener* listener);
    void				handleRun(void* ptr);
    bool                prepare();
    void                loop();
    bool                process(AVMessage *msg);
    void                stop();
    void                postEvent(int what, int arg1, int arg2);
    void                enqueue(AVMessage *msg);


};

#endif //YTXPLAYER_MESSAGELOOP_H
