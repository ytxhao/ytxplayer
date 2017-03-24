//
// Created by Administrator on 2016/11/30.
//
#define LOG_NDEBUG 0
#define TAG "YTX-VideoRefreshThread-JNI"

#include "ytxplayer/ALog-priv.h"

#include <ytxplayer/gl_engine.h>
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/android_media_YtxMediaPlayer.h>
#include <png.h>
#include "ytxplayer/VideoRefreshController.h"


VideoRefreshController::VideoRefreshController(VideoStateInfo *mVideoStateInfo) {

    last_duration = 0.0;
    duration = 0.0;
    delay = 0.0;
    vp = NULL;
    lastvp = NULL;
    remaining_time = 0.0;
    time = 0.0;
    frame_timer = 0.0;
    this->mVideoStateInfo = mVideoStateInfo;
}

void VideoRefreshController::handleRun(void *ptr) {
    if (!prepare()) {
        ALOGE("Couldn't prepare VideoRefreshController\n");
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
    if (mVideoStateInfo != NULL) {

        if (*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED) {
            mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);

        }

        if (*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_STOPPED ||
            *mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PLAYBACK_COMPLETE) {
            return;
        }
        if (remaining_time > 0.0) {
            av_usleep((unsigned int) (remaining_time * 1000000.0));
        }
        remaining_time = REFRESH_RATE;

        if (mVideoStateInfo->frameQueueVideo->frameQueueNumRemaining() < 1) {
            // nothing to do, no picture to display in the queue

        } else {


            lastvp = mVideoStateInfo->frameQueueVideo->frameQueuePeekLast();
            vp = mVideoStateInfo->frameQueueVideo->frameQueuePeek();


            last_duration = vpDuration(lastvp, vp);
            // delay = last_duration;
            if(mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0){
                delay = computeTargetDelay(last_duration);
            }else{
                delay = last_duration;
            }



            time = av_gettime_relative() / 1000000.0; //获取ff系统时间,单位为秒

            if (time < frame_timer + delay) { //如果当前时间小于(frame_timer+delay)则不去frameQueue取下一帧直接刷新当前帧
                remaining_time = FFMIN(frame_timer + delay - time, remaining_time); //显示下一帧还差多长时间
                return;
            }

            frame_timer += delay; //下一帧需要在这个时间显示
            if (delay > 0 && time - frame_timer > AV_SYNC_THRESHOLD_MAX) {
                frame_timer = time;
            }

            mLock.lock();
            if (!isnan(vp->pts)) {
                mVideoStateInfo->currentTime = (int) (vp->pts * 1000);
                mVideoStateInfo->updateVideoPts(vp->pts, vp->pos, vp->serial);
            }
            mLock.unlock();
            /*
             * 添加subtitles
             */

            if (mVideoStateInfo->streamSubtitle->st) {

                while (mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 0) {
                    sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeek();

                    if (mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 1) {
                        sp2 = mVideoStateInfo->frameQueueSubtitle->frameQueuePeekNext();
                    } else {
                        sp2 = NULL;
                    }


                    if (sp->serial != mVideoStateInfo->pkt_serial_subtitle
                        || (mVideoStateInfo->vidClk->pts >
                            (sp->pts + ((float) sp->sub.end_display_time / 1000)))
                        || (sp2 && mVideoStateInfo->vidClk->pts >
                                   (sp2->pts + ((float) sp2->sub.start_display_time / 1000)))) {
                        mVideoStateInfo->frameQueueSubtitle->frameQueueNext();
                    } else {
                        break;
                    }
                }
            }

            int decodeWidth = mVideoStateInfo->streamVideo->dec_ctx->width;
            int decodeHeight = mVideoStateInfo->streamVideo->dec_ctx->height;
            int y_size = decodeWidth * decodeHeight;
            if (lastvp->out_buffer_video_yuv[0] != NULL &&
                *mVideoStateInfo->mCurrentState != MEDIA_PLAYER_STOPPED) {
//                fwrite(vp->frame->data[0],1,y_size,mVideoStateInfo->fp_yuv);    //Y
//                fwrite(vp->frame->data[1],1,y_size/4,mVideoStateInfo->fp_yuv);  //U
//                fwrite(vp->frame->data[2],1,y_size/4,mVideoStateInfo->fp_yuv);  //V

                bool hasSubtitles = false;
                int i = 0;
                if (mVideoStateInfo->streamSubtitle->st) {
                    if (mVideoStateInfo->frameQueueSubtitle->frameQueueNumRemaining() > 0) {
                        sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeek();

                        if (vp->pts >= sp->pts + ((float) sp->sub.start_display_time / 1000)) {
//                            write_png(mVideoStateInfo->join(mVideoStateInfo->mStorageDir,"ass.png"), sp->imageFrame);
                            hasSubtitles = true;

                            if (mVideoStateInfo->sub_format == 0) {
                                uint8_t *data[4] = {0};
                                int linesize[4] = {0};
                                mLock.lock();
                                data[0] = (uint8_t *) lastvp->out_buffer_video_yuv[0];
                                data[1] = (uint8_t *) lastvp->out_buffer_video_yuv[1];
                                data[2] = (uint8_t *) lastvp->out_buffer_video_yuv[2];

                                linesize[0] = lastvp->linesize[0];
                                linesize[1] = lastvp->linesize[1];
                                linesize[2] = lastvp->linesize[2];


                                for (i = 0; i < sp->sub.num_rects; i++) {
                                    blend_subrect(data, linesize, sp->subrects[i], decodeWidth,
                                                  decodeHeight);
                                }
                                mLock.unlock();
                            }
                        }
                    }
                }


                android_media_player_notifyRenderFrame(mVideoStateInfo->VideoGlSurfaceViewObj);
                if (hasSubtitles) {
                    addRendererVideoFrame(mVideoStateInfo->GraphicRendererObj,
                                          sp->imageFrame,
                                          lastvp->out_buffer_video_yuv[0],
                                          lastvp->out_buffer_video_yuv[1],
                                          lastvp->out_buffer_video_yuv[2],
                                          decodeWidth,
                                          decodeHeight);
                } else {
                    addRendererVideoFrame(mVideoStateInfo->GraphicRendererObj,
                                          NULL,
                                          lastvp->out_buffer_video_yuv[0],
                                          lastvp->out_buffer_video_yuv[1],
                                          lastvp->out_buffer_video_yuv[2],
                                          decodeWidth,
                                          decodeHeight);
                }


            }
            mVideoStateInfo->frameQueueVideo->frameQueueNext();
        }
    }

}

#define ALPHA_BLEND(a, oldp, newp, s)\
((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))

void
VideoRefreshController::blend_subrect(uint8_t **data, int *linesize, const AVSubtitleRect *rect,
                                      int imgw, int imgh) {
    int x, y, Y, U, V, A;
    uint8_t *lum, *cb, *cr;
    int dstx, dsty, dstw, dsth;
    const AVSubtitleRect *src = rect;

    dstw = av_clip(rect->w, 0, imgw);
    dsth = av_clip(rect->h, 0, imgh);
    dstx = av_clip(rect->x, 0, imgw - dstw);
    dsty = av_clip(rect->y, 0, imgh - dsth);
    lum = data[0] + dstx + dsty * linesize[0];
    cb = data[1] + dstx / 2 + (dsty >> 1) * linesize[1];
    cr = data[2] + dstx / 2 + (dsty >> 1) * linesize[2];

    for (y = 0; y < dsth; y++) {
        for (x = 0; x < dstw; x++) {
            Y = src->data[0][x + y * src->linesize[0]];
            A = src->data[3][x + y * src->linesize[3]];
            lum[0] = ALPHA_BLEND(A, lum[0], Y, 0);
            lum++;
        }
        lum += linesize[0] - dstw;
    }

    for (y = 0; y < dsth / 2; y++) {
        for (x = 0; x < dstw / 2; x++) {
            U = src->data[1][x + y * src->linesize[1]];
            V = src->data[2][x + y * src->linesize[2]];
            A = src->data[3][2 * x + 2 * y * src->linesize[3]]
                + src->data[3][2 * x + 1 + 2 * y * src->linesize[3]]
                + src->data[3][2 * x + 1 + (2 * y + 1) * src->linesize[3]]
                + src->data[3][2 * x + (2 * y + 1) * src->linesize[3]];
            cb[0] = ALPHA_BLEND(A >> 2, cb[0], U, 0);
            cr[0] = ALPHA_BLEND(A >> 2, cr[0], V, 0);
            cb++;
            cr++;
        }
        cb += linesize[1] - dstw / 2;
        cr += linesize[2] - dstw / 2;
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
        ALOGE("PNG Error opening %s for writing!\n", fname);
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
    if ((ret = wait()) != 0) {
        ALOGE("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}

void VideoRefreshController::refresh() {

    while (mRunning) {
        process();
    }
    //结束视频刷新
    resetRendererVideoFrame(mVideoStateInfo->GraphicRendererObj);
    android_media_player_notifyRenderFrame(mVideoStateInfo->VideoGlSurfaceViewObj);

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


double VideoRefreshController::computeTargetDelay(double delay) {
    double sync_threshold, diff = 0;

    /* update delay to follow master synchronisation source */

    diff = mVideoStateInfo->getClock(mVideoStateInfo->vidClk) -
           mVideoStateInfo->getClock(mVideoStateInfo->audClk);

    /* skip or repeat frame. We take into account the
       delay to compute the threshold. I still don't know
       if it is the best guess */
    sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
    if (!isnan(diff) && fabs(diff) < mVideoStateInfo->max_frame_duration) {

        if (diff <= -sync_threshold) {
            //如果音频播放比视频快
            delay = FFMAX(0, delay + diff);
        } else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD) {
            //如果视频比音频快
            delay = delay + diff;
        } else if (diff >= sync_threshold) {

            delay = 2 * delay;
        }

    }

    //     ALOGI("video: delay=%0.3lf V-A=%lf \n", delay, diff);

    return delay;

}