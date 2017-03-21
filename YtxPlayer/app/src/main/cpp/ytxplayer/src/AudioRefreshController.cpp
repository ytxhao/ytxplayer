//
// Created by Administrator on 2017/1/3.
//

#include "ytxplayer/AudioRefreshController.h"

#define LOG_NDEBUG 0
#define TAG "YTX-AudioRefreshController-JNI"

#include "ytxplayer/ALog-priv.h"
#include "ytxplayer/ffmsg.h"
#include <map>
#include <unistd.h>

//static std::map<SLAndroidSimpleBufferQueueItf,AudioRefreshController*>   audioEnMap;
static AudioRefreshController *mAudioRefreshController;

AudioRefreshController::AudioRefreshController(VideoStateInfo *mVideoStateInfo) {
    ALOGI("AudioRefreshController()\n");
    last_duration = 0;
    duration = 0;
    delay = 0;
    //vp = NULL;
    //lastvp = NULL;
    remaining_time = 0.0;
    time = 0.0;
    frame_timer = 0.0;
    audio_callback_time = 0;

    this->mVideoStateInfo = mVideoStateInfo;
    mAudioEngine = new AudioEngine();
    mAudioRefreshController = this;
}


AudioRefreshController::~AudioRefreshController() {
    ALOGI("~AudioRefreshController\n");
    delete mAudioEngine;
    mAudioRefreshController = NULL;

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
    // audioEnMap.insert(std::pair<SLAndroidSimpleBufferQueueItf,AudioRefreshController*>(mAudioEngine->bqPlayerBufferQueue, this));
    mAudioEngine->RegisterCallback(bqPlayerCallback);
    return true;
}

bool AudioRefreshController::process(AVMessage *msg) {

    ALOGI("AudioRefreshController::process in");
    bool ret = true;
    if (msg->what == FFP_MSG_CANCEL) {
        ret = false;
    }

    switch (msg->what) {
        case FFP_MSG_AUDIO_FIRST_FRAME:
            (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
            audioFrameProcess();
            break;
        case FFP_MSG_COMPLETED:
            (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
            mVideoStateInfo->frameQueueAudio->frameQueueReset();
            break;
    }
    ALOGI("AudioRefreshController::process out");
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
    (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
}

void AudioRefreshController::stop() {

    int ret = -1;
   // (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
    mVideoStateInfo->messageQueueAudio->abort();
    mRunning = false;
    if ((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}


void AudioRefreshController::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
//    AudioRefreshController* sAudioRefreshController = NULL;

    //  ALOGI("AudioRefreshController::bqPlayerCallback pthread_self:%lu,bqPlayerBufferQueue=%#x\n", (long)pthread_self(), bq);
    //  ALOGI("AudioRefreshController::bqPlayerCallback pthread_self:%lu,getpid:%lu,gettid:%lu\n", (long)pthread_self(), (long)getpid(),(long)gettid());
    /*
    std::map<SLAndroidSimpleBufferQueueItf,AudioRefreshController*>::iterator l_it;
    l_it = audioEnMap.find(bq);
    if(l_it == audioEnMap.end()){
        ALOGI("AudioRefreshController::bqPlayerCallback we do not find");

    } else {
      //  ALOGI("AudioRefreshController::bqPlayerCallback we find");
        sAudioRefreshController = l_it->second;
    }
*/
    assert(bq == mAudioRefreshController->mAudioEngine->bqPlayerBufferQueue);
    assert(NULL == context);

    mAudioRefreshController->audioFrameProcess();
//    if (*mAudioRefreshController->mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED) {
//        mAudioRefreshController->mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);
//    }

}

int AudioRefreshController::audioFrameProcess() {
    int ret = 0;
    Frame *af;
    double audio_clock;
    SLuint32 size_buff = 0;
    int64_t audio_callback_time=0;
    char *buf[128]={0};


    if (*mAudioRefreshController->mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED) {
        (*mAudioEngine->bqPlayerBufferQueue)->Enqueue(mAudioEngine->bqPlayerBufferQueue,
                                                      buf,
                                                      128);

    }else{
        mAudioEngine->mLock.lock();
        audio_callback_time = av_gettime_relative();
        af = audioDecodeFrame();
//    ALOGI("AudioRefreshController::process  out_buffer_audio=%#x", af->out_buffer_audio);
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

        if(mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] < 0){
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