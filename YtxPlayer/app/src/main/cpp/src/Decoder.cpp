//
// Created by Administrator on 2016/9/7.
//
#include <ytxplayer/InputStream.h>
#include "ytxplayer/Decoder.h"

#define TAG "FFMpegIDecoder"
#include "ALog-priv.h"

IDecoder::IDecoder(VideoStateInfo *mVideoStateInfo)
{
    this->mVideoStateInfo = mVideoStateInfo;
    mQueue = new PacketQueue(mVideoStateInfo);
    pkt_serial = 0;
}

IDecoder::~IDecoder()
{
    if(mRunning)
    {
        stop();
    }
    free(mQueue);

}

void IDecoder::enqueue(MAVPacket* mPacket)
{
    ALOGI("IDecoder mVideoStateInfo->flushPkt->pkt.data=%#x mPacket->pkt.data=%#x\n",mVideoStateInfo->flushPkt->pkt.data,mPacket->pkt.data);
    mQueue->put(mPacket);
}

void IDecoder::enqueueNullPacket(int stream_index) {
    mQueue->putNullPacket(stream_index);
}

void IDecoder::flush() {
    mQueue->flush();
}

int IDecoder::packets()
{
    return mQueue->size();
}

void IDecoder::stop()
{
//    mQueue->abort();
//    ALOGI("waiting on end of decoder thread\n");
//    int ret = -1;
//    if((ret = wait()) != 0) {
//        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
//        return;
//    }
}

void IDecoder::handleRun(void* ptr)
{
    if(!prepare())
    {
        ALOGI("Couldn't prepare decoder\n");
        return;
    }
    decode(ptr);
}

bool IDecoder::prepare()
{
    return false;
}

bool IDecoder::process(AVPacket *packet)
{
    return false;
}

bool IDecoder::decode(void* ptr)
{
    return false;
}

