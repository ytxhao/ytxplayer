//
// Created by Administrator on 2016/12/6.
//
#define NDEBUG 0
#define TAG "MessageLoop"


#include "ytxplayer/ALog-priv.h"
#include "ytxplayer/MessageLoop.h"
#include "ytxplayer/ffmsg.h"

MessageLoop::MessageLoop() {
    mQueue = new MessageQueue();
    this->listener = NULL;

}

void MessageLoop::setMsgListener(MediaPlayerListener *listener) {
    this->listener = listener;
}

MessageLoop::~MessageLoop() {

    free(mQueue);
}

void MessageLoop::stop() {
    mQueue->abort();
    ALOGI("waiting on end of MessageLoop thread\n");
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel MessageLoop: %i\n", ret);
        return;
    }
}

void MessageLoop::enqueue(AVMessage *msg) {
    mQueue->put(msg);
}

void MessageLoop::handleRun(void *ptr) {
    if(!prepare())
    {
        ALOGI("Couldn't prepare VideoRefreshController\n");
        return;
    }
    loop();
}

void MessageLoop::loop() {

    AVMessage msg;

    while(mRunning){
        if(mQueue->get(&msg,true) < 0){
            mRunning = false;
        }else{
            if(!process(&msg))
            {
                mRunning = false;
            }
        }
    }
}

bool MessageLoop::prepare() {

    return true;
}

bool MessageLoop::process(AVMessage *msg) {
    ALOGI("MessageLoop::process in");
    bool ret = true;
    if(msg->what == FFP_MSG_CANCEL){
        ret = false;
    }
    if(!listener){
        return true;
    }
    switch (msg->what){
        case FFP_MSG_FLUSH:
           // listener->notify(MEDIA_PREPARED, 0, 0);
            break;
        case FFP_MSG_ERROR:
            //post_event();
            listener->notify(MEDIA_ERROR, MEDIA_ERROR_OPEN_STREAM, 0);
            break;
        case FFP_MSG_PREPARED:
            //post_event();
            ALOGI("MessageLoop::process FFP_MSG_PREPARED");
            listener->notify(MEDIA_PREPARED, 0, 0);
            break;
        case FFP_MSG_COMPLETED:
            ALOGI("MessageLoop::process FFP_MSG_COMPLETED");
            listener->notify(MEDIA_PLAYBACK_COMPLETE, 0, 0);
            //post_event();
            break;
        case FFP_MSG_VIDEO_SIZE_CHANGED:
           // post_event();
            break;
        case FFP_MSG_SAR_CHANGED:
            //post_event();
            break;
        case FFP_MSG_VIDEO_RENDERING_START:
           // post_event();
            break;
        case FFP_MSG_AUDIO_RENDERING_START:
           // post_event();
            break;
        case FFP_MSG_VIDEO_ROTATION_CHANGED:
           // post_event();
            break;
        case FFP_MSG_BUFFERING_START:
          //  post_event();
            break;
        case FFP_MSG_BUFFERING_END:
          //  post_event();
            break;
        case FFP_MSG_BUFFERING_UPDATE:
         //   post_event();
            break;
        case FFP_MSG_BUFFERING_BYTES_UPDATE:
        //    post_event();
            break;
        case FFP_MSG_BUFFERING_TIME_UPDATE:
         //   post_event();
            break;
        case FFP_MSG_SEEK_COMPLETE:
         //   post_event();
            break;
        case FFP_MSG_PLAYBACK_STATE_CHANGED:
         //   post_event();
            break;
    }
    ALOGI("MessageLoop::process out");
    return ret;

}

