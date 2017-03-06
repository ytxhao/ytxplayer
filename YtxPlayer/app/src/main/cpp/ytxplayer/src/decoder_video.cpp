//
// Created by Administrator on 2016/9/7.
//

#include <ytxplayer/ffmsg.h>
#include <assert.h>
#include <ytxplayer/utils.h>
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




        if (   last_w != mFrame->width
            || last_h != mFrame->height
            || last_format != mFrame->format
            || last_serial != mVideoStateInfo->pkt_serial_video
            || last_vfilter_idx != mVideoStateInfo->vfilter_idx) {
            ALOGI("Video frame changed from size:%dx%d format:%s serial:%d to size:%dx%d format:%s serial:%d\n",
                   last_w, last_h,(const char *)av_x_if_null(av_get_pix_fmt_name(last_format),"none"),last_serial,
                  mFrame->width, mFrame->height,
                  (const char *)av_x_if_null(av_get_pix_fmt_name((enum AVPixelFormat)mFrame->format), "none"), mVideoStateInfo->vfilter_idx);


            avfilter_graph_free(&graph);
            graph = avfilter_graph_alloc();

            configure_video_filters(graph, mVideoStateInfo->vfilters_list ? mVideoStateInfo->vfilters_list[mVideoStateInfo->vfilter_idx] : NULL, mFrame);


            filt_in  = mVideoStateInfo->in_video_filter;
            filt_out = mVideoStateInfo->out_video_filter;
            last_w = mFrame->width;
            last_h = mFrame->height;
            last_format = (AVPixelFormat) mFrame->format;
            last_serial = mVideoStateInfo->pkt_serial_video;
            last_vfilter_idx = mVideoStateInfo->vfilter_idx;
       //     frameRate = filt_out->inputs[0]->frame_rate;
        }

        ret = av_buffersrc_add_frame(filt_in, mFrame);
        if (ret < 0){

            ALOGE("av_buffersrc_add_frame return err");
            assert(ret >= 0);
        }


        while (ret >= 0) {
            mVideoStateInfo->frame_last_returned_time = av_gettime_relative() / 1000000.0;

            ret = av_buffersink_get_frame_flags(filt_out, mFrame, 0);
            if (ret < 0) {
                if (ret == AVERROR_EOF){
                    mVideoStateInfo->viddec_finished = mVideoStateInfo->pkt_serial_video;
                }

                ret = 0;
                break;
            }

            mVideoStateInfo->frame_last_filter_delay =
                    av_gettime_relative() / 1000000.0 - mVideoStateInfo->frame_last_returned_time;
            if (fabs(mVideoStateInfo->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0)
                mVideoStateInfo->frame_last_filter_delay = 0;
            timeBase = filt_out->inputs[0]->time_base;


            ///////////////////////////////////////////////////
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

            av_image_copy(mFrameYuv->data, mFrameYuv->linesize, (const uint8_t **)mFrame->data, mFrame->linesize,
                          (AVPixelFormat) mFrame->format, vp->width, vp->height);


//            av_image_fill_arrays(mFrameYuv->data, mFrameYuv->linesize,out_buffer_video,
//                                 AV_PIX_FMT_YUV420P, mVideoStateInfo->streamVideo->dec_ctx->width,
//                                 mVideoStateInfo->streamVideo->dec_ctx->height,1);
//
//            sws_scale(mConvertCtx,
//              (const unsigned char *const *) mFrame->data,
//                      mFrame->linesize,
//                      0,
//                      mVideoStateInfo->streamVideo->dec_ctx->height,
//                      mFrameYuv->data,
//                      mFrameYuv->linesize);

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
        }
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
    graph = avfilter_graph_alloc();
    filt_out = NULL;
    filt_in = NULL;

    if (!graph) {
        av_frame_free(&mFrame);
        return AVERROR(ENOMEM);
    }

    if (!mFrame) {
        avfilter_graph_free(&graph);
        return AVERROR(ENOMEM);
    }

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


int DecoderVideo::configure_video_filters(AVFilterGraph *graph, const char *vfilters, AVFrame *frame){


        static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
        char sws_flags_str[512] = "";
        char buffersrc_args[256];
        int ret;
        AVFilterContext *filt_src = NULL, *filt_out = NULL, *last_filter = NULL;
        AVCodecParameters *codecpar =  mVideoStateInfo->streamVideo->st->codecpar;
        AVRational fr = av_guess_frame_rate(mVideoStateInfo->pFormatCtx, mVideoStateInfo->streamVideo->st, NULL);
        AVDictionaryEntry *e = NULL;

        while ((e = av_dict_get(mVideoStateInfo->sws_dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
            if (!strcmp(e->key, "sws_flags")) {
                av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", "flags", e->value);
            } else{
                av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", e->key, e->value);
            }

        }
        if (strlen(sws_flags_str))
            sws_flags_str[strlen(sws_flags_str)-1] = '\0';

        graph->scale_sws_opts = av_strdup(sws_flags_str);

        snprintf(buffersrc_args, sizeof(buffersrc_args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 frame->width, frame->height, frame->format,
                 mVideoStateInfo->streamVideo->st->time_base.num, mVideoStateInfo->streamVideo->st->time_base.den,
                 codecpar->sample_aspect_ratio.num, FFMAX(codecpar->sample_aspect_ratio.den, 1));
        if (fr.num && fr.den)
            av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);

        if ((ret = avfilter_graph_create_filter(&filt_src,
                                                avfilter_get_by_name("buffer"),
                                                "ffplay_buffer", buffersrc_args, NULL,
                                                graph)) < 0){
            goto fail;
        }


        ret = avfilter_graph_create_filter(&filt_out,
                                           avfilter_get_by_name("buffersink"),
                                           "ffplay_buffersink", NULL, NULL, graph);
        if (ret < 0)
            goto fail;

        if ((ret = av_opt_set_int_list(filt_out, "pix_fmts", pix_fmts,  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
            goto fail;

        last_filter = filt_out;

/* Note: this macro adds a filter before the lastly added filter, so the
 * processing order of the filters is in reverse */

//#define INSERT_FILT(name, arg) do {                                          \
//    AVFilterContext *filt_ctx;                                               \
//                                                                             \
//    ret = avfilter_graph_create_filter(&filt_ctx,                            \
//                                       avfilter_get_by_name(name),           \
//                                       "ffplay_" name, arg, NULL, graph);    \
//    if (ret < 0)                                                             \
//        goto fail;                                                           \
//                                                                             \
//    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
//    if (ret < 0)                                                             \
//        goto fail;                                                           \
//                                                                             \
//    last_filter = filt_ctx;                                                  \
//} while (0)
//
//        /* SDL YUV code is not handling odd width/height for some driver
//         * combinations, therefore we crop the picture to an even width/height. */
//        INSERT_FILT("crop", "floor(in_w/2)*2:floor(in_h/2)*2");
//
//        if (mVideoStateInfo->autorotate) {
//            double theta  = get_rotation(mVideoStateInfo->streamVideo->st);
//
//            ALOGI("fabs(theta - 90)=%f",fabs(theta - 90));
//            ALOGI("fabs(theta - 180)=%f",fabs(theta - 180));
//            ALOGI("fabs(theta - 270)=%f",fabs(theta - 270));
//            ALOGI("fabs(theta)=%f",fabs(theta));
//            if (fabs(theta - 90) < 1.0) {
//                INSERT_FILT("transpose", "clock");
//            } else if (fabs(theta - 180) < 1.0) {
//                INSERT_FILT("hflip", NULL);
//                INSERT_FILT("vflip", NULL);
//            } else if (fabs(theta - 270) < 1.0) {
//                INSERT_FILT("transpose", "cclock");
//            } else if (fabs(theta) > 1.0) {
//                char rotate_buf[64];
//                snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
//                INSERT_FILT("rotate", rotate_buf);
//            }
//        }

        if ((ret = configure_filtergraph(graph, vfilters, filt_src, last_filter)) < 0){
            ALOGE("configure_filtergraph ret=%d",ret);
            goto fail;
        }


        mVideoStateInfo->in_video_filter  = filt_src;
        mVideoStateInfo->out_video_filter = filt_out;

        fail:
        return ret;

}
int DecoderVideo::configure_filtergraph(AVFilterGraph *graph, const char *filtergraph,
                          AVFilterContext *source_ctx, AVFilterContext *sink_ctx){


        int ret, i;
        int nb_filters = graph->nb_filters;
        AVFilterInOut *outputs = NULL, *inputs = NULL;

        if (filtergraph) {
            outputs = avfilter_inout_alloc();
            inputs  = avfilter_inout_alloc();
            if (!outputs || !inputs) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }

            outputs->name       = av_strdup("in");
            outputs->filter_ctx = source_ctx;
            outputs->pad_idx    = 0;
            outputs->next       = NULL;

            inputs->name        = av_strdup("out");
            inputs->filter_ctx  = sink_ctx;
            inputs->pad_idx     = 0;
            inputs->next        = NULL;

            if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL)) < 0)
                goto fail;
        } else {
            if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0)
                goto fail;
        }

        /* Reorder the filters to ensure that inputs of the custom filters are merged first */
        for (i = 0; i < graph->nb_filters - nb_filters; i++)
            FFSWAP(AVFilterContext*, graph->filters[i], graph->filters[i + nb_filters]);

        ret = avfilter_graph_config(graph, NULL);
        fail:
        avfilter_inout_free(&outputs);
        avfilter_inout_free(&inputs);
        return ret;

}