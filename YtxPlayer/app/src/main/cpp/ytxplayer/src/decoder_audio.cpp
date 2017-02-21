//
// Created by Administrator on 2016/9/22.
//

#include <android/log.h>
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/ffmsg.h>
#include "ytxplayer/decoder_audio.h"
#define TAG "FFMpegAudioDecoder"
#include "ytxplayer/ALog-priv.h"

DecoderAudio::DecoderAudio(VideoStateInfo *mVideoStateInfo):IDecoder(mVideoStateInfo)
{
   // isFirstFrame = true;
   // this->mVideoStateInfo = mVideoStateInfo;
    firstInit = false;
    mVideoStateInfo->initClock(mVideoStateInfo->audClk,&mQueue->serial);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->audClk,1);

}

DecoderAudio::~DecoderAudio()
{
    // Free audio samples buffer
    if(mFrame != NULL){
        av_frame_free(&mFrame);
    }
    avcodec_close(mVideoStateInfo->streamAudio->dec_ctx);
}

bool DecoderAudio::prepare()
{

    ALOGI("ytxhao DecoderAudio::prepare\n");
    mConvertCtx = swr_alloc();
    swr_alloc_set_opts(mConvertCtx,
                       mVideoStateInfo->out_ch_layout,mVideoStateInfo->out_sample_fmt,mVideoStateInfo->out_sample_rate,
                       mVideoStateInfo->in_ch_layout,mVideoStateInfo->in_sample_fmt,mVideoStateInfo->in_sample_rate,
                       0, NULL);

    swr_init(mConvertCtx);
 //   16bit 44100 PCM 数据
    mVideoStateInfo->out_buffer_audio = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }

    return true;
}

bool DecoderAudio::process(MAVPacket *mPacket)
{
    int pts = 0;
    int size = mSamplesSize;
    int	completed;
    int i=0;
    int j=0;
//    ALOGI("DecoderAudio::process mPacket->isEnd=%d",mPacket->isEnd);
//    if(mPacket->isEnd){
//        return false;
//    }

    curStats = mPacket->isEnd;
//    ALOGI("DecoderAudio::process mPacket->isEnd=%d curStats=%d lastStats=%d",mPacket->isEnd,curStats,lastStats);

    if(curStats != lastStats && curStats && mPacket->pkt.data == NULL){
        msg.what = FFP_MSG_COMPLETED;
        mVideoStateInfo->messageQueueAudio->put(&msg);
        //  fclose(mVideoStateInfo->fp_yuv);
        lastStats = curStats;
        return true;
    }
    lastStats = curStats;

    if(mQueue->size() == 0){
        pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    }

    if(mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data){
        avcodec_flush_buffers(mVideoStateInfo->streamAudio->dec_ctx);
        mVideoStateInfo->isFirstAudioFrame = true;
        return true;
    }

    if(mVideoStateInfo->pkt_serial_audio != mQueue->serial){
        return true;
    }


    if(mPacket->pkt.size == 0 && mPacket->pkt.data == NULL){
        return true;
    }

    int ret = avcodec_decode_audio4(mVideoStateInfo->streamAudio->dec_ctx,mFrame,&completed,&mPacket->pkt);
  //  ALOGI("DecoderAudio::process ret=%d ; completed=%d \n",ret,completed);
    //call handler for posting buffer to os audio driver

    if (completed > 0) {
        //   pts = synchronize(mFrame, pts);

       // onDecode(mFrame, pts);
   //     ALOGI("DecoderAudio::process 0 mFrame->sample_rate=%d mFrame->pts=%lf",mFrame->sample_rate,(double)mFrame->pts);
        tb = (AVRational){1, mFrame->sample_rate};

        if (mFrame->pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process mFrame->pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(mFrame->pts, mVideoStateInfo->streamAudio->dec_ctx->time_base, tb);
        } else if (mFrame->pkt_pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process mFrame->pkt_pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(mFrame->pkt_pts, av_codec_get_pkt_timebase(mVideoStateInfo->streamAudio->dec_ctx), tb);
        } else if (next_pts != AV_NOPTS_VALUE){
            ALOGI("DecoderAudio::process next_pts != AV_NOPTS_VALUE");
            mFrame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
        }

        if (mFrame->pts != AV_NOPTS_VALUE) {
            next_pts = mFrame->pts + mFrame->nb_samples;
            next_pts_tb = tb;
        }

//        ALOGI("DecoderAudio::process 1 mFrame->sample_rate=%d mFrame->pts=%lf",mFrame->sample_rate,(double)mFrame->pts);
//////////////////////////////////////////////////////////////////////
        if(!(af = mVideoStateInfo->frameQueueAudio->frameQueuePeekWritable())){
            return true;
        }

        tb = (AVRational){1, mFrame->sample_rate};
        af->pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(tb);
        af->pos = av_frame_get_pkt_pos(mFrame);
        af->serial = mVideoStateInfo->pkt_serial_audio;
        af->duration = av_q2d((AVRational){mFrame->nb_samples, mFrame->sample_rate});
       // ALOGI("DecoderAudio::process mFrame->sample_rate=%d af->pts=%lf af->pos=%d af->duration=%lf",mFrame->sample_rate,af->pts,af->pos,af->duration);
       // ALOGI("DecoderAudio::process test mFrame->data[0]=%#x",mFrame->data[0]);
      //  av_frame_move_ref(af->frame, mFrame);


        swr_convert(mConvertCtx, &(mVideoStateInfo->out_buffer_audio), MAX_AUDIO_FRAME_SIZE,
                    (const uint8_t **) mFrame->data, mFrame->nb_samples);
        //获取sample的size
        af->out_buffer_audio_size = av_samples_get_buffer_size(NULL, mVideoStateInfo->out_channel_nb,
                                                         mFrame->nb_samples, mVideoStateInfo->out_sample_fmt,
                                                         1);

        af->out_buffer_audio = (uint8_t *)av_malloc(af->out_buffer_audio_size);



        memcpy(af->out_buffer_audio,mVideoStateInfo->out_buffer_audio,af->out_buffer_audio_size);

        mVideoStateInfo->frameQueueAudio->frameQueuePush();
        av_frame_unref(mFrame);
        if(mVideoStateInfo->isFirstAudioFrame && mVideoStateInfo->frameQueueAudio->windex > 3){
            for(i=0;i<SAMPLE_QUEUE_SIZE;i++){

                ALOGI("FrameQueue::process yuhaotest queue[%d].out_buffer_audio=%#x",i,mVideoStateInfo->frameQueueAudio->queue[i].out_buffer_audio);

                fwrite(mVideoStateInfo->frameQueueAudio->queue[i].out_buffer_audio, 1,
                       (size_t) mVideoStateInfo->frameQueueAudio->queue[i].out_buffer_audio_size, mVideoStateInfo->fp_pcm);
            }
            mVideoStateInfo->isFirstAudioFrame  = false;
            msg.what = FFP_MSG_AUDIO_FIRST_FRAME;
            mVideoStateInfo->messageQueueAudio->put(&msg);
        }
    }

    return true;
}

bool DecoderAudio::decode(void* ptr)
{
    int i;
    MAVPacket        pPacket;

    ALOGI( TAG, "decoding audio");

    while(mRunning)
    {
        if(mQueue->get(&pPacket, true,&mVideoStateInfo->pkt_serial_audio) < 0)
        {
            mRunning = false;
            return false;
        }
        if(!process(&pPacket))
        {
            mRunning = false;
            return false;
        }
        // Free the packet that was allocated by av_read_frame
        av_packet_unref(&pPacket.pkt);
    }

    ALOGI( TAG, "decoding audio ended");

    return true;
}

void DecoderAudio::stop() {
    mRunning = false;
    mQueue->abort();
    mVideoStateInfo->frameQueueAudio->frameQueueReset();
    ALOGI("waiting on end of decoder thread\n");
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}

int DecoderAudio::streamHasEnoughPackets(){
    int ret = 0;
    ret = mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] < 0 ||
          mQueue->mAbortRequest ||
          (mVideoStateInfo->streamAudio->st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
          mQueue->size() > MIN_FRAMES && (!mQueue->duration || av_q2d(mVideoStateInfo->streamAudio->st->time_base) * mQueue->duration > 1.0);

    return ret;
}