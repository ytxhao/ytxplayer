//
// Created by Administrator on 2017/1/3.
//
#define LOG_NDEBUG 0
#define TAG "YTX-AudioRefreshController-JNI"

#include <unistd.h>
#include "ytxplayer/AudioRefreshController.h"
#include "ytxplayer/ALog-priv.h"
#include "ytxplayer/ffmsg.h"

AudioRefreshController::AudioRefreshController(VideoStateInfo *mVideoStateInfo) {

    last_duration = 0;
    duration = 0;
    delay = 0;
    remaining_time = 0.0;
    time = 0.0;
    frame_timer = 0.0;
    audio_callback_time = 0;

    this->mVideoStateInfo = mVideoStateInfo;
    mAudioEngine = new AudioEngine();
}


AudioRefreshController::~AudioRefreshController() {

    delete mAudioEngine;

}

void AudioRefreshController::handleRun(void *ptr) {
    if (!prepare()) {
        ALOGI("Couldn't prepare AudioRefreshController\n");
        return;
    }
    refresh();
}

bool AudioRefreshController::prepare() {
    mAudioEngine->createEngine();
    mAudioEngine->createBufferQueueAudioPlayer(mVideoStateInfo->streamAudio->dec_ctx->sample_rate,
                                               960, mVideoStateInfo->out_channel_nb);
    ALOGI("AudioRefreshController::prepare pthread_self:%lu,bqPlayerBufferQueue=%#x\n",
          (long) pthread_self(), mAudioEngine->bqPlayerBufferQueue);

    mAudioEngine->RegisterCallback(bqPlayerCallback, this);
    return true;
}

bool AudioRefreshController::process(AVMessage *msg) {
    SLresult result;
    bool ret = true;
    if (msg->what == FFP_MSG_CANCEL) {
        ret = false;
    }

   // ALOGI("AudioRefreshController msg->what=%d",msg->what);
    switch (msg->what) {
        case FFP_MSG_AUDIO_FIRST_FRAME:
            mAudioEngine->setMuteAudioPlayer(true);
           // mAudioEngine->setPauseAudioPlayer(true);
           // result = (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
            SLAndroidSimpleBufferQueueState recQueueState;
            result = (*mAudioEngine->bqPlayerBufferQueue)->GetState(mAudioEngine->bqPlayerBufferQueue, &recQueueState);
            ALOGI("AudioRefreshController QueueState.count=%d QueueState.index=%d",recQueueState.count,recQueueState.index);
            if(recQueueState.index == 0){
                result = (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
                audioFrameProcess();
            }

            //mAudioEngine->setPauseAudioPlayer(false);
            mAudioEngine->setMuteAudioPlayer(false);
            break;
        case FFP_MSG_COMPLETED:
            mAudioEngine->setMuteAudioPlayer(true);
            result = (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
            mVideoStateInfo->frameQueueAudio->frameQueueReset();
            break;
    }

    return ret;


}

void AudioRefreshController::refresh() {
    AVMessage msg;
    while (mRunning) {
        if (mVideoStateInfo->messageQueueAudio->get(&msg, true) < 0) {
            mRunning = false;
        } else {
            if (!process(&msg)) {
                mRunning = false;
            }
        }
    }
    // mAudioEngine->setPlayingAudioPlayer(false);
    mAudioEngine->setMuteAudioPlayer(true);
    (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
}

void AudioRefreshController::stop() {

    int ret = -1;
    // (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
    mVideoStateInfo->messageQueueAudio->abort();
    mRunning = false;
    if ((ret = wait()) != 0) {
        ALOGE("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}


void AudioRefreshController::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioRefreshController* sAudioRefreshController = NULL;

    assert(context != NULL);
    sAudioRefreshController = (AudioRefreshController *) context;
    assert(bq == sAudioRefreshController->mAudioEngine->bqPlayerBufferQueue);

    sAudioRefreshController->audioFrameProcess();

}

int AudioRefreshController::audioFrameProcess() {
    int ret = 0;
    Frame *af;
    double audio_clock;
    SLuint32 size_buff = 0;
    int64_t audio_callback_time = 0;
    char *buf[128] = {0};

//    if (*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED) {
//        mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);
//    }

    if (*mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED) {
        (*mAudioEngine->bqPlayerBufferQueue)->Enqueue(mAudioEngine->bqPlayerBufferQueue,
                                                      buf,
                                                      128);

    } else {
        mAudioEngine->mLock.lock();
        audio_callback_time = av_gettime_relative();
        af = audioDecodeFrame();

//    /* update the audio clock with the pts */
//    if (!isnan(af->pts))
//        audio_clock = af->pts + (double) af->frame->nb_samples / af->frame->sample_rate;
//    else
//        audio_clock = NAN;

//    swr_convert(mVideoStateInfo->swrCtx, &(mVideoStateInfo->out_buffer_audio), MAX_AUDIO_FRAME_SIZE,
//                (const uint8_t **) af->frame->data, af->frame->nb_samples);
//    //获取sample的size
//    int out_buffer_size = av_samples_get_buffer_size(NULL, mVideoStateInfo->out_channel_nb,
//                                                     af->frame->nb_samples, mVideoStateInfo->out_sample_fmt,
//                                                     1);
        size_buff = (SLuint32) af->out_buffer_audio_size;
        // fwrite(af->out_buffer_audio,1,size_buff,mVideoStateInfo->fp_pcm1);
        (*mAudioEngine->bqPlayerBufferQueue)->Enqueue(mAudioEngine->bqPlayerBufferQueue,
                                                      af->out_buffer_audio,
                                                      size_buff);
        mAudioEngine->mLock.unlock();

        if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] < 0) {
            mVideoStateInfo->currentTime = (int) (af->pts * 1000);
        }

        if (!isnan(audio_clock)) {

            mVideoStateInfo->setClockAt(mVideoStateInfo->audClk, af->pts, af->serial,
                                        audio_callback_time / 1000000.0);
            mVideoStateInfo->syncClock2Slave(mVideoStateInfo->extClk, mVideoStateInfo->audClk);
        }
    }


    return ret;
}


Frame *AudioRefreshController::audioDecodeFrame() {

    Frame *af;
    do {

        do {
            af = mVideoStateInfo->frameQueueAudio->frameQueuePeekReadable();
            mVideoStateInfo->frameQueueAudio->frameQueueNext();
        } while (af == NULL);

    } while (af->serial != mVideoStateInfo->pkt_serial_audio);

    return af;

}

void AudioRefreshController::enqueue(AVMessage *msg) {
    mVideoStateInfo->messageQueueAudio->put(msg);
}