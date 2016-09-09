//
// Created by Administrator on 2016/9/7.
//
#include "decoder.h"

#define TAG "FFMpegIDecoder"

IDecoder::IDecoder(InputStream* stream)
{
    mQueue = new PacketQueue();
    mStream = stream;
   // this->stream = stream;
}

IDecoder::~IDecoder()
{
    if(mRunning)
    {
        stop();
    }
    free(mQueue);
    avcodec_close(mStream->dec_ctx);
}

void IDecoder::enqueue(AVPacket* packet,int *i)
{
    mQueue->put(packet,i);
}

int IDecoder::packets()
{
    return mQueue->size();
}

void IDecoder::stop()
{
    mQueue->abort();
    ALOGI("waiting on end of decoder thread\n");
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
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

