//
// Created by Administrator on 2016/9/7.
//

#include <ytxplayer/ffmsg.h>
#include "ytxplayer/decoder_video.h"

#define TAG "FFMpegVideoDecoder"
#include "ytxplayer/ALog-priv.h"
static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

DecoderVideo::DecoderVideo(VideoStateInfo *mVideoStateInfo):IDecoder(mVideoStateInfo)
{
    ALOGI("ytxhao DecoderVideo::DecoderVideo\n");
    firstInit = false;
    mVideoStateInfo->initClock(mVideoStateInfo->vidClk,&mQueue->serial);
    mVideoStateInfo->initClock(mVideoStateInfo->extClk,&mVideoStateInfo->extClk->serial);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->vidClk,1);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->extClk,1);

    sprintf(file,"%s%lu.yuv",mVideoStateInfo->mStorageDir,pthread_self());
    ALOGI("ytxhao DecoderVideo file = %s",file);
   // mVideoStateInfo->fp_yuv = fopen(file,"wb+");
    mFrame = NULL;
    out_buffer_video = NULL;
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        ALOGE("mFrame == NULL");
    }
    mFrameYuv = NULL;
    mFrameYuv = av_frame_alloc();
    if (mFrameYuv == NULL){
        ALOGE("mFrameYuv == NULL");
    }

}

DecoderVideo::~DecoderVideo()
{
    ALOGI("ytxhao DecoderVideo::~DecoderVideo\n");
    //mVideoStateInfo->frameQueueVideo->frameQueueDestroy();

    // Free the RGB image
    if(!mFrame){
        av_frame_free(&mFrame);
        mFrame = NULL;
    }

    if(!mFrameYuv){
        av_frame_free(&mFrameYuv);
        mFrameYuv = NULL;
    }

    if(!mConvertCtx){
        sws_freeContext(mConvertCtx);
    }

}

bool DecoderVideo::prepare()
{
    ALOGI("ytxhao DecoderVideo::prepare\n");
    mConvertCtx = sws_getContext(mVideoStateInfo->streamVideo->dec_ctx->width,
                                 mVideoStateInfo->streamVideo->dec_ctx->height,
                                 mVideoStateInfo->streamVideo->dec_ctx->pix_fmt,
                                 mVideoStateInfo->streamVideo->dec_ctx->width,
                                 mVideoStateInfo->streamVideo->dec_ctx->height,
                                 AV_PIX_FMT_YUV420P,
                                 SWS_BICUBIC,
                                 NULL,
                                 NULL,
                                 NULL);
    out_buffer_video=(unsigned char *)av_malloc((size_t) av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                                                  mVideoStateInfo->streamVideo->dec_ctx->width,
                                                                                  mVideoStateInfo->streamVideo->dec_ctx->height, 1));

    return true;
}

double DecoderVideo::synchronize(AVFrame *src_frame, double pts) {

    double frame_delay;

    if (pts != 0) {
        /* if we have pts, set video clock to it */
        mVideoClock = pts;
    } else {
        /* if we aren't given a pts, set it to the clock */
        pts = mVideoClock;
    }
    /* update the video clock */
    frame_delay = av_q2d( mVideoStateInfo->streamVideo->dec_ctx->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    mVideoClock += frame_delay;
    return pts;
}

bool DecoderVideo::process(MAVPacket *mPacket)
{
    int	completed;
    double pts = 0;
    int ret=0;
    // Decode video frame

    curStats = mPacket->isEnd;
    ALOGI("DecoderVideo::process mPacket->isEnd=%d curStats=%d lastStats=%d",mPacket->isEnd,curStats,lastStats);

    if(curStats != lastStats && curStats && mPacket->pkt.data == NULL){
        AVMessage msg;
        msg.what = FFP_MSG_COMPLETED;
        mVideoStateInfo->mMessageLoop->enqueue(&msg);
      //  fclose(mVideoStateInfo->fp_yuv);
        lastStats = curStats;
        return true;
    }
    lastStats = curStats;

    if(mQueue->size() == 0){
        pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    }

    if(mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data){
        avcodec_flush_buffers(mVideoStateInfo->streamVideo->dec_ctx);
        return true;
    }

    if(mVideoStateInfo->pkt_serial_video != mQueue->serial){
        return true;
    }

    if(mPacket->pkt.size == 0 && mPacket->pkt.data == NULL){
        return true;
    }


    ret = avcodec_decode_video2( mVideoStateInfo->streamVideo->dec_ctx,
                         mFrame,
                         &completed,
                                 &mPacket->pkt);

    if (completed) {
//        ALOGI("####T 0 mFrame=%#x \n",mFrame);
        int size_y = 0;
        double duration;
        mFrame->pts = av_frame_get_best_effort_timestamp(mFrame);
        duration = (frameRate.num && frameRate.den ? av_q2d((AVRational){frameRate.den, frameRate.num}) : 0);
        pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(timeBase);

        mVideoStateInfo->mVideoWidth  = mFrame->width;
        mVideoStateInfo->mVideoHeight = mFrame->height;

        Frame *vp;
        if(!(vp = mVideoStateInfo->frameQueueVideo->frameQueuePeekWritable())){
            return true;
        }

        if(vp->reallocate || !vp->allocated ||
           vp->width  != mFrame->width ||
           vp->height != mFrame->height){

            vp->allocated  = 0;
            vp->reallocate = 0;
          //  ALOGI("fef0=%#x mFrame=%#x \n",vp,mFrame);
          //  ALOGI("####T vp->width=%d mFrame->width=%d \n",vp->width,mFrame->width);
            vp->width = mFrame->width;
            vp->height = mFrame->height;
        }

        vp->pts = pts;
        vp->duration = duration;
        vp->serial = mVideoStateInfo->pkt_serial_video;
        vp->pos = av_frame_get_pkt_pos(mFrame);


        av_image_fill_arrays(mFrameYuv->data, mFrameYuv->linesize,out_buffer_video,
                             AV_PIX_FMT_YUV420P, mVideoStateInfo->streamVideo->dec_ctx->width,
                             mVideoStateInfo->streamVideo->dec_ctx->height,1);

        sws_scale(mConvertCtx,
          (const unsigned char *const *) mFrame->data,
                  mFrame->linesize,
                  0,
                  mVideoStateInfo->streamVideo->dec_ctx->height,
                  mFrameYuv->data,
                  mFrameYuv->linesize);

        size_y = mVideoStateInfo->streamVideo->dec_ctx->width *  mVideoStateInfo->streamVideo->dec_ctx->height;

        vp->out_buffer_video_yuv[0] = (char *) malloc(sizeof(char) * size_y);
        vp->out_buffer_video_yuv[1] = (char *) malloc(sizeof(char) * size_y / 4);
        vp->out_buffer_video_yuv[2] = (char *) malloc(sizeof(char) * size_y / 4);

        memcpy(vp->out_buffer_video_yuv[0], mFrameYuv->data[0], (size_t) size_y);
        memcpy(vp->out_buffer_video_yuv[1], mFrameYuv->data[1], (size_t) (size_y / 4));
        memcpy(vp->out_buffer_video_yuv[2], mFrameYuv->data[2], (size_t) (size_y / 4));

        mVideoStateInfo->frameQueueVideo->frameQueuePush();
        av_frame_unref(mFrame);
        av_frame_unref(mFrameYuv);
        return true;
    }



    return true;
}

bool DecoderVideo::decode(void* ptr)
{
    MAVPacket        pPacket;

    int i;
    ALOGI( "decoding video\n");
  //  AVFrame *frame = av_frame_alloc();
     timeBase =  mVideoStateInfo->streamVideo->st->time_base;//is->video_st->time_base;
     frameRate = av_guess_frame_rate(mVideoStateInfo->pFormatCtx, mVideoStateInfo->streamVideo->st, NULL);



    while(mRunning)
    {
        if(mQueue->get(&pPacket, true,&mVideoStateInfo->pkt_serial_video) < 0)
        {
            mRunning = false;
           // return false;
        }else{

            if(!process(&pPacket))
            {
                mRunning = false;
                return false;
            }
            // Free the packet that was allocated by av_read_frame
            av_packet_unref(&pPacket.pkt);
        }
        ALOGI( "DecoderVideo::decode mQueue->size()=%d\n",mQueue->size());
    }

    ALOGI("decoding video ended\n");


    return true;
}

/* These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int DecoderVideo::getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags) {
    int ret = avcodec_default_get_buffer2(c, pic,flags);
    uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
    *pts = global_video_pkt_pts;
    pic->opaque = pts;
    return ret;
}
void DecoderVideo::releaseBuffer(struct AVCodecContext *c, AVFrame *pic) {
    if (pic)
        av_freep(&pic->opaque);
   // avcodec_default_release_buffer(c, pic);
}



void DecoderVideo::stop() {
    mRunning = false;
    mQueue->abort();
    mVideoStateInfo->frameQueueVideo->frameQueueReset();
    ALOGI("waiting on end of decoder thread\n");
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}


int DecoderVideo::streamHasEnoughPackets(){
    int ret = 0;
    ret = mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] < 0 ||
            mQueue->mAbortRequest ||
            (mVideoStateInfo->streamVideo->st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
            mQueue->size() > MIN_FRAMES && (!mQueue->duration || av_q2d(mVideoStateInfo->streamVideo->st->time_base) * mQueue->duration > 1.0);

    return ret;
}