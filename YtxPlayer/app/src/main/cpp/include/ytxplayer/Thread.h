//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_THREAD_H
#define YTXPLAYER_THREAD_H

#include <pthread.h>

class Thread
{
public:
    Thread();
    ~Thread();

    void						start();
    void						startAsync();
    int							wait();

    void 						waitOnNotify();
    void						notify();
    virtual void				stop();

protected:
    volatile  bool				mRunning;

    virtual void                handleRun(void* ptr);

private:
    pthread_t                   mThread;
    pthread_mutex_t     		mLock;
    pthread_cond_t				mCondition;

    static void*				startThread(void* ptr);
};

#endif //YTXPLAYER_THREAD_H
