//
// Created by Administrator on 2016/9/7.
//
#define LOG_NDEBUG 0
#define TAG "FFMpegIDecoder"

#include "ytxplayer/ALog-priv.h"

#include <ytxplayer/InputStream.h>
#include "ytxplayer/Decoder.h"

IDecoder::IDecoder(VideoStateInfo *mVideoStateInfo) {
    this->mVideoStateInfo = mVideoStateInfo;
    mQueue = new PacketQueue(mVideoStateInfo);
}

IDecoder::~IDecoder() {
    if (mRunning) {
        stop();
    }
    free(mQueue);

}

void IDecoder::enqueue(MAVPacket *mPacket) {
    mQueue->put(mPacket);
}

void IDecoder::enqueueNullPacket(int stream_index) {
    mQueue->putNullPacket(stream_index);
}

void IDecoder::flush() {
    mQueue->flush();
}

int IDecoder::packets() {
    return mQueue->size();
}

void IDecoder::stop() {
//    mQueue->abort();
//    ALOGI("waiting on end of decoder thread\n");
//    int ret = -1;
//    if((ret = wait()) != 0) {
//        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
//        return;
//    }
}

void IDecoder::handleRun(void *ptr) {
    if (!prepare()) {
        ALOGE("Couldn't prepare decoder\n");
        return;
    }
    decode(ptr);
}

bool IDecoder::prepare() {
    return false;
}

bool IDecoder::process(AVPacket *packet) {
    return false;
}

bool IDecoder::decode(void *ptr) {
    return false;
}

