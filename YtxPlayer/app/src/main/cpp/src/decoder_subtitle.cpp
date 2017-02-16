//
// Created by Administrator on 2016/9/22.
//

#include <android/log.h>
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/ffmsg.h>
#include "ytxplayer/decoder_subtitle.h"
#define TAG "FFMpegSubtitleDecoder"
#include "ALog-priv.h"

DecoderSubtitle::DecoderSubtitle(VideoStateInfo *mVideoStateInfo):IDecoder(mVideoStateInfo)
{
   // isFirstFrame = true;
   // this->mVideoStateInfo = mVideoStateInfo;
    firstInit = false;

}

DecoderSubtitle::~DecoderSubtitle()
{
    // Free audio samples buffer
    if(mFrame != NULL){
        av_frame_free(&mFrame);
    }
    if(!mConvertCtx){
        sws_freeContext(mConvertCtx);
    }

    //avcodec_close(mVideoStateInfo->streamSubtitle->dec_ctx);
}

bool DecoderSubtitle::prepare()
{

    ALOGI("DecoderSubtitle::prepare\n");
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }
    return true;
}

bool DecoderSubtitle::process(MAVPacket *mPacket)
{
    double pts = 0;
    int size = mSamplesSize;
    int	completed;
    curStats = mPacket->isEnd;
    if(curStats != lastStats && curStats && mPacket->pkt.data == NULL){
        lastStats = curStats;
        return true;
    }
    lastStats = curStats;

    if(mQueue->size() == 0){
        pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    }

    if(mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data){
        avcodec_flush_buffers(mVideoStateInfo->streamSubtitle->dec_ctx);
        return true;
    }

    if(mVideoStateInfo->pkt_serial_subtitle != mQueue->serial){
        return true;
    }


    if(mPacket->pkt.size == 0 && mPacket->pkt.data == NULL){
        return true;
    }

    if (!(sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeekWritable())) {
        return true;
    }



    int ret = avcodec_decode_subtitle2(mVideoStateInfo->streamSubtitle->dec_ctx, &sp->sub, &completed, &mPacket->pkt);

    if (completed > 0  && sp->sub.format == 0) {
        if (sp->sub.pts != AV_NOPTS_VALUE) {
            pts = sp->sub.pts / (double) AV_TIME_BASE;
        }
        sp->pts = pts;
        sp->serial = mVideoStateInfo->pkt_serial_subtitle;

        if (!(sp->subrects = (AVSubtitleRect **) av_mallocz_array(sp->sub.num_rects, sizeof(AVSubtitleRect*)))) {
            ALOGI("Cannot allocate subrects\n");
            exit(1);
        }

        for (int i = 0; i < sp->sub.num_rects; i++)
        {
            int in_w = sp->sub.rects[i]->w;
            int in_h = sp->sub.rects[i]->h;
            int subw = mVideoStateInfo->streamSubtitle->dec_ctx->width  ? mVideoStateInfo->streamSubtitle->dec_ctx->width  : mVideoStateInfo->mVideoWidth;
            int subh = mVideoStateInfo->streamSubtitle->dec_ctx->height ? mVideoStateInfo->streamSubtitle->dec_ctx->height : mVideoStateInfo->mVideoHeight;
            int out_w = mVideoStateInfo->mVideoWidth  ? in_w * mVideoStateInfo->mVideoWidth  / subw : in_w;
            int out_h = mVideoStateInfo->mVideoHeight ? in_h * mVideoStateInfo->mVideoHeight / subh : in_h;

            if (!(sp->subrects[i] = (AVSubtitleRect *) av_mallocz(sizeof(AVSubtitleRect))) ||
                av_image_alloc(sp->subrects[i]->data, sp->subrects[i]->linesize, out_w, out_h, AV_PIX_FMT_YUVA420P, 16) < 0) {
                ALOGI("Cannot allocate subtitle data\n");
                exit(1);
            }

            mConvertCtx = sws_getCachedContext(mConvertCtx,
                                                       in_w, in_h, AV_PIX_FMT_PAL8, out_w, out_h,
                                                       AV_PIX_FMT_YUVA420P, sws_flags, NULL, NULL, NULL);
            if (!mConvertCtx) {
                ALOGI("Cannot initialize the sub conversion context\n");
                exit(1);
            }
            sws_scale(mConvertCtx,
                      (const uint8_t *const *) sp->sub.rects[i]->data, sp->sub.rects[i]->linesize,
                      0, in_h, sp->subrects[i]->data, sp->subrects[i]->linesize);

            sp->subrects[i]->w = out_w;
            sp->subrects[i]->h = out_h;
            sp->subrects[i]->x = sp->sub.rects[i]->x * out_w / in_w;
            sp->subrects[i]->y = sp->sub.rects[i]->y * out_h / in_h;


        }
        /* now we can update the picture count */
        mVideoStateInfo->frameQueueSubtitle->frameQueuePush();


    }else if(completed > 0 && sp->sub.format == 1){

        for (int i = 0; i < sp->sub.num_rects; i++){

                    ALOGI("ttttt sp->sub.rects[i]->type=%d, sp->sub.rects[i]->ass=%s\n",sp->sub.rects[i]->type,sp->sub.rects[i]->ass);
        }


    }else{
         avsubtitle_free(&sp->sub);
    }

    return true;
}

bool DecoderSubtitle::decode(void* ptr)
{
    int i;
    MAVPacket        pPacket;

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

    return true;
}

void DecoderSubtitle::stop() {
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

int DecoderSubtitle::streamHasEnoughPackets(){
    int ret = 0;
    ret = mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] < 0 ||
          mQueue->mAbortRequest ||
          (mVideoStateInfo->streamAudio->st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
          mQueue->size() > MIN_FRAMES && (!mQueue->duration || av_q2d(mVideoStateInfo->streamAudio->st->time_base) * mQueue->duration > 1.0);

    return ret;
}