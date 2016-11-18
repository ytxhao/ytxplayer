//
// Created by Administrator on 2016/11/18.
//

#ifndef YTXPLAYER_LOCK_H
#define YTXPLAYER_LOCK_H


class Lock{
private:
    pthread_mutex_t mLock;
    pthread_cond_t mCondition;
public:
    Lock();
    ~Lock();
    void lock();
    void unlock();
    void condWait();
    void condSignal();

};

#endif //YTXPLAYER_LOCK_H
