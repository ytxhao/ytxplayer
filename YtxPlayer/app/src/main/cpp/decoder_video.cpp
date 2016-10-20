//
// Created by Administrator on 2016/9/7.
//




#include "decoder_video.h"

#define TAG "FFMpegVideoDecoder"

static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

DecoderVideo::DecoderVideo(InputStream* stream) : IDecoder(stream)
{
   // stream->dec_ctx->get_buffer2 = getBuffer;
  //  mStream->codec->
   // stream->dec_ctx->release_buffer = releaseBuffer;
  //  frameQueue = new FrameQueue();
    frameQueueInitFinsh = frameQueue.frameQueueInit(VIDEO_PICTURE_QUEUE_SIZE,1,stream);
}

DecoderVideo::~DecoderVideo()
{
}

bool DecoderVideo::prepare()
{
    ALOGI("ytxhao DecoderVideo::prepare\n");
    mConvertCtx = sws_getContext(mStream->dec_ctx->width,
                                 mStream->dec_ctx->height,
                                 mStream->dec_ctx->pix_fmt,
                                 mStream->dec_ctx->width,
                                 mStream->dec_ctx->height,
                                 AV_PIX_FMT_YUV420P,
                                 SWS_BICUBIC,
                                 NULL,
                                 NULL,
                                 NULL);
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
    frame_delay = av_q2d(mStream->dec_ctx->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    mVideoClock += frame_delay;
    return pts;
}

bool DecoderVideo::process(AVPacket *packet, int *i)
{
    int	completed;
    double pts = 0;
    int ret=0;
    // Decode video frame

//
    if(*i == 1 && mQueue->size()==0){
        onDecodeFinish();
        return false;
    }

    ret = avcodec_decode_video2(mStream->dec_ctx,
                         mFrame,
                         &completed,
                         packet);

//
//    if (packet->dts == AV_NOPTS_VALUE && mFrame->opaque
//        && *(uint64_t*) mFrame->opaque != AV_NOPTS_VALUE) {
//        pts = *(uint64_t *) mFrame->opaque;
//    } else if (packet->dts != AV_NOPTS_VALUE) {
//        pts = packet->dts;
//    } else {
//        pts = 0;
//    }
//    pts *= av_q2d(mStream->dec_ctx->time_base);



    if (completed) {
      //  pts = synchronize(mFrame, pts);
    //    double dpts = NAN;
        double duration;

        mFrame->pts = av_frame_get_best_effort_timestamp(mFrame);

//        if (mFrame->pts != AV_NOPTS_VALUE)
//            dpts = av_q2d(mStream->st->time_base) * mFrame->pts;
//        else
//            ALOGI("DecoderVideo::process mFrame->pts == AV_NOPTS_VALUE");

//        ALOGI("DecoderVideo::process ret=%d ; completed=%d mFrame->pts=%lld av_gettime_relative()=%lf\n",ret,completed,mFrame->pts,av_gettime_relative()/1000000.0);
//        ALOGI("DecoderVideo::process av_q2d(mStream->dec_ctx->time_base)=%lf\n",av_q2d(mStream->dec_ctx->time_base));
//        ALOGI("DecoderVideo::process av_q2d(mStream->dec_ctx->time_base)*mFrame->pts=%lf\n",av_q2d(mStream->dec_ctx->time_base)*mFrame->pts);
//
//        ALOGI("DecoderVideo::process av_q2d(mStream->st->time_base)=%lf\n",av_q2d(mStream->st->time_base));
//        ALOGI("DecoderVideo::process av_q2d(mStream->st->time_base)*mFrame->pts=%lf\n",av_q2d(mStream->st->time_base)*mFrame->pts);

        duration = (frameRate.num && frameRate.den ? av_q2d((AVRational){frameRate.den, frameRate.num}) : 0);
        pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(timeBase);

        ALOGI("DecoderVideo::process duration=%lf pts=%lf",duration,pts);

        Frame *vp;
        if(!(vp = frameQueue.frameQueuePeekWritable())){
            return true;
        }
 //       vp->sar = mFrame->sample_aspect_ratio;

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
   //     vp->pos = av_frame_get_pkt_pos(mFrame);
      //  vp->frame = mFrame;
     //   vp->serial = serial;
//        out_buffer_video=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  mStream->dec_ctx->width, mStream->dec_ctx->height,1));
//        av_image_fill_arrays(vp->frame->data, vp->frame->linesize,out_buffer_video,
//                             AV_PIX_FMT_YUV420P,mStream->dec_ctx->width, mStream->dec_ctx->height,1);

        out_buffer_video=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  mStream->dec_ctx->width, mStream->dec_ctx->height,1));
        av_image_fill_arrays(vp->frame->data, vp->frame->linesize,out_buffer_video,
                             AV_PIX_FMT_YUV420P,mStream->dec_ctx->width, mStream->dec_ctx->height,1);

        sws_scale(mConvertCtx,
          (const unsigned char *const *) mFrame->data,
                  mFrame->linesize,
                  0,
                  mStream->dec_ctx->height,
                  vp->frame->data,
                  vp->frame->linesize);



        onDecode(mFrame, pts);
        frameQueue.frameQueuePush();
        av_frame_unref(mFrame);
        return true;
    }



    return true;
}

bool DecoderVideo::decode(void* ptr)
{
    AVPacket        pPacket;

    int i;
    ALOGI( "decoding video\n");
  //  AVFrame *frame = av_frame_alloc();
     timeBase = mStream->st->time_base;//is->video_st->time_base;
     frameRate = av_guess_frame_rate(mStream->pFormatCtx, mStream->st, NULL);



    while(mRunning)
    {
        if(mQueue->get(&pPacket, true, &i) < 0)
        {
            mRunning = false;
           // return false;
        }else{

            if(!process(&pPacket,&i))
            {
                mRunning = false;
                return false;
            }
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&pPacket);
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

