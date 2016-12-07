//
// Created by Administrator on 2016/12/2.
//

#ifndef YTXPLAYER_PLAYERPREPAREASYNC_H
#define YTXPLAYER_PLAYERPREPAREASYNC_H

#include "Thread.h"

class PlayerPrepareAsync : public Thread{
public:
    void				handleRun(void* ptr);
    bool                prepare();
    void                process();


};


#endif //YTXPLAYER_PLAYERPREPAREASYNC_H
