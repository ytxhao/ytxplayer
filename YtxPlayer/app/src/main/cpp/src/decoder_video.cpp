//
// Created by Administrator on 2016/9/7.
//





#include "ytxplayer/decoder_video.h"

#define TAG "FFMpegVideoDecoder"
#include "ALog-priv.h"
static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

DecoderVideo::DecoderVideo(VideoStateInfo *mVideoStateInfo):IDecoder(mVideoStateInfo)
{
   // this->mVideoStateInfo = mVideoStateInfo;
    mVideoStateInfo->initClock(mVideoStateInfo->vidClk,&mQueue->serial);
    mVideoStateInfo->initClock(mVideoStateInfo->extClk,&mVideoStateInfo->extClk->serial);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->vidClk,1);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->extClk,1);
}

DecoderVideo::~DecoderVideo()
{
    mVideoStateInfo->frameQueueVideo->frameQueueDestory();
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
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }
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
    ALOGI("DecoderVideo::process completed un 0.0");
//
    ALOGI("DecoderVideo::process mPacket->isEnd=%d",mPacket->isEnd);
    if(mPacket->isEnd){
        onDecodeFinish();
        return false;
    }
    ALOGI("DecoderVideo::process1 mPacket->pkt.data=%#x mVideoStateInfo->flushPkt->pkt.data=%#x  pkt_serial=%d mQueue->serial=%d \n",
          mPacket->pkt.data,mVideoStateInfo->flushPkt->pkt.data,pkt_serial,mQueue->serial);

    if(mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data){
        avcodec_flush_buffers(mVideoStateInfo->streamVideo->dec_ctx);
        return true;
    }

    if(pkt_serial != mQueue->serial){
        return true;
    }
    ALOGI("DecoderVideo::process completed un 0");
    ret = avcodec_decode_video2( mVideoStateInfo->streamVideo->dec_ctx,
                         mFrame,
                         &completed,
                                 &mPacket->pkt);
    ALOGI("DecoderVideo::process completed un 1");
    if (completed) {

        double duration;
        mFrame->pts = av_frame_get_best_effort_timestamp(mFrame);
        duration = (frameRate.num && frameRate.den ? av_q2d((AVRational){frameRate.den, frameRate.num}) : 0);
        pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(timeBase);

        ALOGI("DecoderVideo::process completed");
        ALOGI("DecoderVideo::process duration=%lf pts=%lf",duration,pts);



        Frame *vp;
        if(!(vp = mVideoStateInfo->frameQueueVideo->frameQueuePeekWritable())){
            return true;
        }

        if(vp->reallocate || !vp->allocated ||
           vp->width  != mFrame->width ||
           vp->height != mFrame->height){

            vp->allocated  = 0;
            vp->reallocate = 0;
            vp->width = mFrame->width;
            vp->height = mFrame->height;
        }

        vp->pts = pts;
        vp->duration = duration;
        vp->serial = pkt_serial;
        vp->pos = av_frame_get_pkt_pos(mFrame);



   //     out_buffer_video=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  mStream->dec_ctx->width, mStream->dec_ctx->height,1));
        av_image_fill_arrays(vp->frame->data, vp->frame->linesize,out_buffer_video,
                             AV_PIX_FMT_YUV420P, mVideoStateInfo->streamVideo->dec_ctx->width,
                             mVideoStateInfo->streamVideo->dec_ctx->height,1);

        sws_scale(mConvertCtx,
          (const unsigned char *const *) mFrame->data,
                  mFrame->linesize,
                  0,
                  mVideoStateInfo->streamVideo->dec_ctx->height,
                  vp->frame->data,
                  vp->frame->linesize);


        onDecode(mFrame, pts);
        mVideoStateInfo->frameQueueVideo->frameQueuePush();
        av_frame_unref(mFrame);
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
        if(mQueue->get(&pPacket, true,&pkt_serial) < 0)
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
    // Free the RGB image
    av_free(mFrame);

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

