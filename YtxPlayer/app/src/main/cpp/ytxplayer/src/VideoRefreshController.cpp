//
// Created by Administrator on 2016/11/30.
//

#include <ytxplayer/gl_engine.h>
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/android_media_YtxMediaPlayer.h>
#include <png.h>
#include "ytxplayer/VideoRefreshController.h"
#define LOG_NDEBUG 0
#define TAG "YTX-VideoRefreshThread-JNI"
#include "ytxplayer/ALog-priv.h"

VideoRefreshController::VideoRefreshController(VideoStateInfo* mVideoStateInfo)
{
    this->mVideoStateInfo = mVideoStateInfo;
}

void VideoRefreshController::handleRun(void* ptr){
    if(!prepare())
    {
        ALOGI("Couldn't prepare VideoRefreshController\n");
        return;
    }
    refresh();
}

bool VideoRefreshController::prepare() {
    return true;
}

#define REFRESH_RATE 0.01
#define AV_SYNC_THRESHOLD_MAX 0.1
void VideoRefreshController::process() {
    Frame *sp, *sp2;
    if(mVideoStateInfo != NULL) {

        if(*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED){
            mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);

        }

        if(*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_STOPPED || *mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PLAYBACK_COMPLETE){
            return;
        }
        if (remaining_time > 0.0) {
            ALOGI("startPlayerRefresh remaining_time=%lf  remaining_time*1000000=%lf\n",remaining_time,remaining_time * 1000000.0);
            av_usleep((unsigned int) (remaining_time * 1000000.0));
        }
        remaining_time = REFRESH_RATE;

        ALOGI("startPlayerRefresh mVideoStateInfo=%#x",mVideoStateInfo);
        ALOGI("startPlayerRefresh frameQueueVideo=%#x",mVideoStateInfo->frameQueueVideo);
        if (mVideoStateInfo->frameQueueVideo->frameQueueNumRemaining() < 1) {
            // nothing to do, no picture to display in the queue

        } else {


            //  ALOGI("startPlayerRefresh mDecoderVideo->frameQueue.size=%d\n",sPlayer->mDecoderVideo->frameQueue.size);
            //  ALOGI("startPlayerRefresh frameQueueNumRemaining size=%d\n",sPlayer->mDecoderVideo->frameQueue.frameQueueNumRemaining());
            lastvp = mVideoStateInfo->frameQueueVideo->frameQueuePeekLast();
            vp = mVideoStateInfo->frameQueueVideo->frameQueuePeek();


            last_duration = vpDuration(lastvp, vp);
           // delay = last_duration;
            delay = computeTargetDelay(last_duration);


            time = av_gettime_relative() / 1000000.0; //获取ff系统时间,单位为秒
            ALOGI("startPlayerRefresh last_duration=%lf:time=%lf:frame_timer=%lf:frame_timer+delay=%lf,pFormatCtx->start_time=%lf,pFormatCtx->streams video start_time=%lf vp->pts=%lf\n",
                  last_duration, time, frame_timer, frame_timer + delay,mVideoStateInfo->pFormatCtx->start_time,mVideoStateInfo->pFormatCtx->streams[mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO]]->start_time,vp->pts);

            if (time < frame_timer + delay) { //如果当前时间小于(frame_timer+delay)则不去frameQueue取下一帧直接刷新当前帧
                remaining_time = FFMIN(frame_timer + delay - time, remaining_time); //显示下一帧还差多长时间
                return;
            }

            frame_timer += delay; //下一帧需要在这个时间显示
            if (delay > 0 && time - frame_timer > AV_SYNC_THRESHOLD_MAX) {
                frame_timer = time;
            }

            if (!isnan(vp->pts)) {
                mVideoStateInfo->currentTime = (int) (vp->pts * 1000);
                mVideoStateInfo->updateVideoPts(vp->pts, vp->pos, vp->serial);
            }

            /*
             * 添加subtitles
             */

            if(mVideoStateInfo->streamSubtitle->st){

                while(mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 0){
                    sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeek();

                    if(mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 1){
                        sp2 = mVideoStateInfo->frameQueueSubtitle->frameQueuePeekNext();
                    }else{
                        sp2 = NULL;
                    }


                    if (sp->serial != mVideoStateInfo->pkt_serial_subtitle
                        || (mVideoStateInfo->vidClk->pts > (sp->pts + ((float) sp->sub.end_display_time / 1000)))
                        || (sp2 && mVideoStateInfo->vidClk->pts > (sp2->pts + ((float) sp2->sub.start_display_time / 1000))))
                    {
                        mVideoStateInfo->frameQueueSubtitle->frameQueueNext();
                    } else {
                        break;
                    }
                }
            }

          //  display:
            int decodeWidth = mVideoStateInfo->streamVideo->dec_ctx->width;
            int decodeHeight = mVideoStateInfo->streamVideo->dec_ctx->height;
            int y_size = decodeWidth*decodeHeight;
            Frame *vp;
            vp = mVideoStateInfo->frameQueueVideo->frameQueuePeekLast();
            if (vp->out_buffer_video_yuv[0] != NULL && *mVideoStateInfo->mCurrentState != MEDIA_PLAYER_STOPPED) {
//                ALOGI("to getGlEngine()->addRendererFrame %lu decodeWidth=%d decodeHeight=%d",pthread_self(),decodeWidth,decodeHeight);
//                fwrite(vp->frame->data[0],1,y_size,mVideoStateInfo->fp_yuv);    //Y
//                fwrite(vp->frame->data[1],1,y_size/4,mVideoStateInfo->fp_yuv);  //U
//                fwrite(vp->frame->data[2],1,y_size/4,mVideoStateInfo->fp_yuv);  //V

                bool hasSubtitles = false;
                if(mVideoStateInfo->streamSubtitle->st){
                    if(mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 0){
                        sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeek();

                        if (vp->pts >= sp->pts + ((float) sp->sub.start_display_time / 1000)) {
//                            write_png(mVideoStateInfo->join3(mVideoStateInfo->mStorageDir,"ass.png"), sp->imageFrame);
                            hasSubtitles = true;
                        //    addRendererSubtitleFrame(mVideoStateInfo->GraphicRendererObj,sp->imageFrame);
                        }
                    }
                }


                android_media_player_notifyRenderFrame(mVideoStateInfo->VideoGlSurfaceViewObj);
                if(hasSubtitles){
                    addRendererVideoFrame(mVideoStateInfo->GraphicRendererObj,
                                          sp->imageFrame,
                                          vp->out_buffer_video_yuv[0],
                                          vp->out_buffer_video_yuv[1],
                                          vp->out_buffer_video_yuv[2],
                                          decodeWidth,
                                          decodeHeight);
                }else{
                    addRendererVideoFrame(mVideoStateInfo->GraphicRendererObj,
                                          NULL,
                                          vp->out_buffer_video_yuv[0],
                                          vp->out_buffer_video_yuv[1],
                                          vp->out_buffer_video_yuv[2],
                                          decodeWidth,
                                          decodeHeight);
                }



            }
            mVideoStateInfo->frameQueueVideo->frameQueueNext();
        }
    }

}

void VideoRefreshController::write_png(char *fname, image_t *img) {

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte **row_pointers;
    int k;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    fp = NULL;

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    fp = fopen(fname, "wb");
    if (fp == NULL) {
        printf("PNG Error opening %s for writing!\n", fname);
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, 0);

    png_set_IHDR(png_ptr, info_ptr, img->width, img->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_set_bgr(png_ptr);

    row_pointers = (png_byte **) malloc(img->height * sizeof(png_byte *));
    for (k = 0; k < img->height; k++)
        row_pointers[k] = img->buffer + img->stride * k;

    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(row_pointers);

    fclose(fp);
}

void VideoRefreshController::stop() {
    mRunning = false;
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}

void VideoRefreshController::refresh() {

    while(mRunning){
        process();
    }
}

 double VideoRefreshController::vpDuration(Frame *vp, Frame *next_vp) {
    if (vp->serial == next_vp->serial) {
        double duration = next_vp->pts - vp->pts;
        if (isnan(duration) || duration <= 0 || duration > mVideoStateInfo->max_frame_duration)
            return vp->duration;
        else
            return duration;
    } else {
        return 0.0;
    }
}


double  VideoRefreshController::computeTargetDelay(double delay){
        double sync_threshold, diff = 0;

        /* update delay to follow master synchronisation source */

            diff = mVideoStateInfo->getClock(mVideoStateInfo->vidClk) - mVideoStateInfo->getClock(mVideoStateInfo->audClk);

            /* skip or repeat frame. We take into account the
               delay to compute the threshold. I still don't know
               if it is the best guess */
            sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
            ALOGI("video: 000 delay=%0.3lf V-A=%lf sync_threshold=%lf max_frame_duration=%lf\n", delay, diff,sync_threshold,mVideoStateInfo->max_frame_duration);
            if (!isnan(diff) && fabs(diff) < mVideoStateInfo->max_frame_duration) {

                if (diff <= -sync_threshold) {
                    //如果音频播放比视频快
                    ALOGI("video: 000 001");
                    delay = FFMAX(0, delay + diff);
                } else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD){
                    //如果视频比音频快
                    ALOGI("video: 000 002");
                    delay = delay + diff;
                } else if (diff >= sync_threshold){
                    ALOGI("video: 000 003");
                    delay = 2 * delay;
                }

//                if (diff <= -sync_threshold) {
//                    //如果音频播放比视频快
//                    ALOGI("video: 000 001");
//                    delay = FFMAX(0, delay + diff);
//                } else if (diff >= sync_threshold){
//                    //如果视频比音频快
//                    ALOGI("video: 000 002");
//                    delay =  diff;
//                }

            }

        ALOGI("video: 001 delay=%0.3lf V-A=%lf \n", delay, diff);

        return delay;

}