//
// Created by Administrator on 2016/11/30.
//

#include <ytxplayer/gl_engine.h>
#include <ytxplayer/VideoStateInfo.h>
#include "VideoRefreshController.h"
#define LOG_NDEBUG 0
#define TAG "YTX-VideoRefreshThread-JNI"
#include "ALog-priv.h"

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

    if(mVideoStateInfo != NULL) {

        if(*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED){
            mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);

        }
        if (remaining_time > 0.0) {
            //    ALOGI("startPlayerRefresh remaining_time=%lf\n",remaining_time);
            av_usleep((int64_t) (remaining_time * 1000000.0));

        }
        remaining_time = REFRESH_RATE;
        //usleep(20000);

        if (mVideoStateInfo->frameQueueVideo->frameQueueNumRemaining() < 2) {
            // nothing to do, no picture to display in the queue

        } else {


            //  ALOGI("startPlayerRefresh mDecoderVideo->frameQueue.size=%d\n",sPlayer->mDecoderVideo->frameQueue.size);
            //  ALOGI("startPlayerRefresh frameQueueNumRemaining size=%d\n",sPlayer->mDecoderVideo->frameQueue.frameQueueNumRemaining());
            lastvp = mVideoStateInfo->frameQueueVideo->frameQueuePeekLast();
            vp = mVideoStateInfo->frameQueueVideo->frameQueuePeek();


            last_duration = vp_duration(lastvp, vp);
            delay = last_duration;


            time = av_gettime_relative() / 1000000.0; //获取ff系统时间,单位为秒
            ALOGI("startPlayerRefresh last_duration=%lf:time=%lf:frame_timer=%lf:frame_timer+delay=%lf\n",
                  last_duration, time, frame_timer, frame_timer + delay);

            if (time < frame_timer + delay) { //如果当前时间小于(frame_timer+delay)则不去frameQueue取下一帧直接刷新当前帧
                remaining_time = FFMIN(frame_timer + delay - time, remaining_time); //显示下一帧还差多长时间
                //goto display;
               // continue;
                return;
            }

            frame_timer += delay; //下一帧需要在这个时间显示
            if (delay > 0 && time - frame_timer > AV_SYNC_THRESHOLD_MAX) {
                frame_timer = time;
            }


            display:
            int decodeWidth = mVideoStateInfo->streamVideo->dec_ctx->width;
            int decodeHeight = mVideoStateInfo->streamVideo->dec_ctx->height;
          //  int y_size = mDecoderVideo->mStream->dec_ctx->width * mDecoderVideo->mStream->dec_ctx->height;
            Frame *vp;
            vp = mVideoStateInfo->frameQueueVideo->frameQueuePeekLast();
            if (vp->frame != NULL) {
                GlEngine::getGlEngine()->addRendererFrame((char *) vp->frame->data[0],
                                                          (char *) vp->frame->data[1],
                                                          (char *) vp->frame->data[2],
                                                          decodeWidth,
                                                          decodeHeight);


                  GlEngine::getGlEngine()->notifyRenderer();


            }
            mVideoStateInfo->frameQueueVideo->frameQueueNext();

        }
    }

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


 double VideoRefreshController::vp_duration(Frame *vp, Frame *next_vp) {
    if (vp->serial == next_vp->serial) {
        double duration = next_vp->pts - vp->pts;
        if (isnan(duration) || duration <= 0)
            return vp->duration;
        else
            return duration;
    } else {
        return 0.0;
    }
}