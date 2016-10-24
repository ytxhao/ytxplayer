//
// Created by Administrator on 2016/9/22.
//

#include <android/log.h>
#include "ytxplayer/decoder_audio.h"

#define TAG "FFMpegAudioDecoder"

DecoderAudio::DecoderAudio(InputStream* stream) : IDecoder(stream)
{

}

DecoderAudio::~DecoderAudio()
{
}

bool DecoderAudio::prepare()
{
    //mSamplesSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;
//    mSamples = (int16_t *) av_malloc(mSamplesSize);
//    if(mSamples == NULL) {
//        return false;
//    }
    ALOGI("ytxhao DecoderAudio::prepare\n");
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }
    return true;
}

bool DecoderAudio::process(AVPacket *packet,int *i)
{
    int pts = 0;
    int size = mSamplesSize;
    int	completed;

    if(*i == 1 && mQueue->size()==0){
        return false;
    }
    //int len = avcodec_decode_audio3(mStream->codec, mSamples, &size, packet);
    int ret = avcodec_decode_audio4(mStream->dec_ctx,mFrame,&completed,packet);
    ALOGI("DecoderAudio::process ret=%d ; completed=%d \n",ret,completed);
    //call handler for posting buffer to os audio driver

    if (completed > 0) {
        //   pts = synchronize(mFrame, pts);

        onDecode(mFrame, pts);

        return true;
    }

    return true;
}

bool DecoderAudio::decode(void* ptr)
{
    int i;
    AVPacket        pPacket;

    ALOGI( TAG, "decoding audio");

    while(mRunning)
    {
        if(mQueue->get(&pPacket, true,&i) < 0)
        {
            mRunning = false;
            return false;
        }
        if(!process(&pPacket,&i))
        {
            mRunning = false;
            return false;
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&pPacket);
    }

    ALOGI( TAG, "decoding audio ended");

    // Free audio samples buffer
    //av_free(mSamples);
    av_frame_free(&mFrame);
    return true;
}

