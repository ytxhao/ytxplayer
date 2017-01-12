//
// Created by Administrator on 2017/1/3.
//

#include "AudioRefreshController.h"

#define LOG_NDEBUG 0
#define TAG "YTX-AudioRefreshController-JNI"
#include "ALog-priv.h"
#include "ffmsg.h"
static AudioRefreshController* sAudioRefreshController = NULL;
AudioRefreshController::AudioRefreshController(VideoStateInfo* mVideoStateInfo)
{

    this->mVideoStateInfo = mVideoStateInfo;
    mAudioEngine = AudioEngine::getAudioEngine();
    sAudioRefreshController = this;
}


AudioRefreshController::~AudioRefreshController(){
//    if(mRunning)
//    {
//        stop();
//    }
}

void AudioRefreshController::handleRun(void* ptr){
    if(!prepare())
    {
        ALOGI("Couldn't prepare AudioRefreshController\n");
        return;
    }
    refresh();
}

bool AudioRefreshController::prepare() {
    mAudioEngine->createEngine();
    mAudioEngine->createBufferQueueAudioPlayer(mVideoStateInfo->streamAudio->dec_ctx->sample_rate,960,mVideoStateInfo->out_channel_nb,bqPlayerCallback);
    return true;
}

bool AudioRefreshController::process(AVMessage *msg) {

    ALOGI("AudioRefreshController::process in");
    bool ret = true;
    if(msg->what == FFP_MSG_CANCEL){
        ret = false;
    }

    switch (msg->what){
        case FFP_MSG_AUDIO_FIRST_FRAME:
            (*mAudioEngine->bqPlayerBufferQueue)->Clear(mAudioEngine->bqPlayerBufferQueue);
            audioFrameProcess();
            break;
    }
    ALOGI("AudioRefreshController::process out");
    return ret;


}

void AudioRefreshController::refresh() {
    AVMessage msg;
    while(mRunning){
        if(mVideoStateInfo->messageQueueAudio->get(&msg,true) < 0){
            mRunning = false;
        }else{
            if(!process(&msg))
            {
                mRunning = false;
            }
        }
    }
}

void AudioRefreshController::stop() {

    int ret = -1;
    mVideoStateInfo->messageQueueAudio->abort();
    mRunning = false;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}


void AudioRefreshController::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == AudioEngine::getAudioEngine()->bqPlayerBufferQueue);
    assert(NULL == context);

    sAudioRefreshController->audioFrameProcess();
    if(*sAudioRefreshController->mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED){
        sAudioRefreshController->mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);

    }

}

int AudioRefreshController::audioFrameProcess() {
    int ret = 0;
    Frame *af;
    double audio_clock;
    mAudioEngine->mLock.lock();
    int64_t audio_callback_time = av_gettime_relative();
    af = audioDecodeFrame();

//    /* update the audio clock with the pts */
//    if (!isnan(af->pts))
//        audio_clock = af->pts + (double) af->frame->nb_samples / af->frame->sample_rate;
//    else
//        audio_clock = NAN;

    swr_convert(mVideoStateInfo->swrCtx, &(mVideoStateInfo->out_buffer_audio), MAX_AUDIO_FRAME_SIZE,
                (const uint8_t **) af->frame->data, af->frame->nb_samples);
    //获取sample的size
    int out_buffer_size = av_samples_get_buffer_size(NULL, mVideoStateInfo->out_channel_nb,
                                                     af->frame->nb_samples, mVideoStateInfo->out_sample_fmt,
                                                     1);

    (*mAudioEngine->bqPlayerBufferQueue)->Enqueue(mAudioEngine->bqPlayerBufferQueue,
                                                  mVideoStateInfo->out_buffer_audio,
                                                  out_buffer_size);
    mAudioEngine->mLock.unlock();

    if (!isnan(audio_clock)) {

        mVideoStateInfo->setClockAt(mVideoStateInfo->audClk, af->pts, af->serial,audio_callback_time/1000000.0);
        mVideoStateInfo->syncClock2Slave(mVideoStateInfo->extClk, mVideoStateInfo->audClk);
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