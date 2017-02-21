//
// Created by Administrator on 2016/12/2.
//
#define LOG_NDEBUG 0
#define TAG "YTX-playerPrepareAsync-JNI"
#include <ytxplayer/ALog-priv.h>
#include "ytxplayer/PlayerPrepareAsync.h"

void PlayerPrepareAsync::handleRun(void *ptr) {
    if(!prepare())
    {
        ALOGI("Couldn't prepare VideoRefreshController\n");
        return;
    }
    process();
}

bool PlayerPrepareAsync::prepare() {
    return true;
}

void PlayerPrepareAsync::process() {



}