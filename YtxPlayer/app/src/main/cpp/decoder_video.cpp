//
// Created by Administrator on 2016/9/7.
//

#include "decoder_video.h"

#define TAG "FFMpegVideoDecoder"

static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

DecoderVideo::DecoderVideo(InputStream* stream) : IDecoder(stream)
{
    stream->dec_ctx->get_buffer2 = getBuffer;
  //  mStream->codec->
  //  mStream->codec->release_buffer = releaseBuffer;
}

DecoderVideo::~DecoderVideo()
{
}

bool DecoderVideo::prepare()
{
    ALOGI("ytxhao DecoderVideo::prepare\n");
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
    ALOGI("process mStream->dec_ctx=%d,mStream=%d\n",mStream->dec_ctx,mStream);
    ALOGI("process mStream->dec_ctx codec_id=%d , i=%d\n",mStream->dec_ctx->codec_id,*i);
    int	completed;
    int pts = 0;
    int ret=0;
    // Decode video frame

//
    if(*i == 0 && mQueue->size()==0){
        onDecodeFinish();
        return false;
    }

    ret = avcodec_decode_video2(mStream->dec_ctx,
                         mFrame,
                         &completed,
                         packet);

    /*
    if (packet->dts == AV_NOPTS_VALUE && mFrame->opaque
        && *(uint64_t*) mFrame->opaque != AV_NOPTS_VALUE) {
        pts = *(uint64_t *) mFrame->opaque;
    } else if (packet->dts != AV_NOPTS_VALUE) {
        pts = packet->dts;
    } else {
        pts = 0;
    }
    pts *= av_q2d(mStream->time_base);
*/

    ALOGI("DecoderVideo::process ret=%d ; completed=%d \n",ret,completed);
    if (completed) {
        ALOGI("process mFrame=%d,mFrame->data=%d,mFrame->data[0]=%d\n",mFrame,mFrame->data,mFrame->data[0]);
     //   pts = synchronize(mFrame, pts);

        onDecode(mFrame, pts);
//        if(ret < 0 || (isFinish == 1 && mQueue->size()==0)){
//            onDecodeFinish();
//            ALOGI("DecoderVideo::process ret=%d\n",ret);
//            return false;
//        }

        return true;
    }



    return true;
}

bool DecoderVideo::decode(void* ptr)
{
    AVPacket        pPacket;

    int i;
    ALOGI( "decoding video\n");

    while(mRunning)
    {

       // ALOGI("1 decoding video pPacket.buf=%d,pPacket.data=%d,pPacket.size=%d\n",pPacket.buf,pPacket.data,pPacket.size);
      //  ALOGI( "x decoding video &pPacket=%d\n",&pPacket);

        if(mQueue->get(&pPacket, true, &i) < 0)
        {
            mRunning = false;
           // return false;
        }else{
            ALOGI( "DecoderVideo::decode mQueue->size()=%d\n",mQueue->size());
            if(!process(&pPacket,&i))
            {
                mRunning = false;
                return false;
            }
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&pPacket);
        }
//        if(isFinish == 1){
//            //mRunning = false;
//            break;
//        }

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

