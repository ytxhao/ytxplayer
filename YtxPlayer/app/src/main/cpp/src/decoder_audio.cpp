//
// Created by Administrator on 2016/9/22.
//

#include <android/log.h>
#include "ytxplayer/decoder_audio.h"

#define TAG "FFMpegAudioDecoder"

DecoderAudio::DecoderAudio(InputStream* stream) : IDecoder(stream)
{
    isFirstFrame = true;
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

       // onDecode(mFrame, pts);
        ALOGI("DecoderAudio::process 0 mFrame->sample_rate=%d mFrame->pts=%lf",mFrame->sample_rate,(double)mFrame->pts);
        tb = (AVRational){1, mFrame->sample_rate};

        if (mFrame->pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process mFrame->pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(mFrame->pts, mStream->dec_ctx->time_base, tb);
        } else if (mFrame->pkt_pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process mFrame->pkt_pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(mFrame->pkt_pts, av_codec_get_pkt_timebase(mStream->dec_ctx), tb);
        } else if (next_pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process next_pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
        }

        if (mFrame->pts != AV_NOPTS_VALUE) {
            next_pts = mFrame->pts + mFrame->nb_samples;
            next_pts_tb = tb;
        }

        ALOGI("DecoderAudio::process 1 mFrame->sample_rate=%d mFrame->pts=%lf",mFrame->sample_rate,(double)mFrame->pts);
//////////////////////////////////////////////////////////////////////
        if(!(af = frameQueue->frameQueuePeekWritable())){
            return true;
        }

        tb = (AVRational){1, mFrame->sample_rate};
        af->pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(tb);
        af->pos = av_frame_get_pkt_pos(mFrame);
      //  af->serial = is->auddec.pkt_serial;
        af->duration = av_q2d((AVRational){mFrame->nb_samples, mFrame->sample_rate});
        ALOGI("DecoderAudio::process mFrame->sample_rate=%d af->pts=%lf af->pos=%d af->duration=%lf",mFrame->sample_rate,af->pts,af->pos,af->duration);
        av_frame_move_ref(af->frame, mFrame);
        frameQueue->frameQueuePush();

        if(isFirstFrame){
            isFirstFrame = false;
            firstFrameHandler();
        }
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

