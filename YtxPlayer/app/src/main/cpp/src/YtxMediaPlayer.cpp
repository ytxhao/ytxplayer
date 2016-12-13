//
// Created by Administrator on 2016/9/2.
//

#define LOG_NDEBUG 0
#define TAG "YTX-PLAYER-JNI"

#include <ytxplayer/VideoRefreshController.h>
#include "ALog-priv.h"
//该文件必须包含在源文件中(*.cpp),以免宏展开时提示重复定义的错误
#include "ytxplayer/YtxMediaPlayer.h"
#include "ytxplayer/gl_engine.h"
#include "ffmsg.h"
#define MAX_AUDIO_FRME_SIZE 48000 * 4
#define FPS_DEBUGGING true

void printferr(){
    ALOGI( "AVERROR_BSF_NOT_FOUND=%d\n",AVERROR_BSF_NOT_FOUND);
    ALOGI( "AVERROR_BUG=%d\n",AVERROR_BUG);
    ALOGI( "AVERROR_BUFFER_TOO_SMALL=%d\n",AVERROR_BUFFER_TOO_SMALL);
    ALOGI( "AVERROR_DECODER_NOT_FOUND=%d\n",AVERROR_DECODER_NOT_FOUND);
    ALOGI( "AVERROR_DEMUXER_NOT_FOUND=%d\n",AVERROR_DEMUXER_NOT_FOUND);
    ALOGI( "AVERROR_ENCODER_NOT_FOUND=%d\n",AVERROR_ENCODER_NOT_FOUND);
    ALOGI( "AVERROR_EOF=%d\n",AVERROR_EOF);
    ALOGI( "AVERROR_EXIT=%d\n",AVERROR_EXIT);
    ALOGI( "AVERROR_EXTERNAL=%d\n",AVERROR_EXTERNAL);
    ALOGI( "AVERROR_FILTER_NOT_FOUND=%d\n",AVERROR_FILTER_NOT_FOUND);
    ALOGI( "AVERROR_INVALIDDATA=%d\n",AVERROR_INVALIDDATA);
    ALOGI( "AVERROR_MUXER_NOT_FOUND=%d\n",AVERROR_MUXER_NOT_FOUND);
    ALOGI( "AVERROR_OPTION_NOT_FOUND=%d\n",AVERROR_OPTION_NOT_FOUND);
    ALOGI( "AVERROR_PATCHWELCOME=%d\n",AVERROR_PATCHWELCOME);
    ALOGI( "AVERROR_PROTOCOL_NOT_FOUND=%d\n",AVERROR_PROTOCOL_NOT_FOUND);
    ALOGI( "AVERROR_STREAM_NOT_FOUND=%d\n",AVERROR_STREAM_NOT_FOUND);
/**
 * This is semantically identical to AVERROR_BUG
 * it has been introduced in Libav after our AVERROR_BUG and with a modified value.
 */
    ALOGI( "AVERROR_BUG2=%d\n",AVERROR_BUG2);
    ALOGI( "AVERROR_UNKNOWN=%d\n",AVERROR_UNKNOWN);
    ALOGI( "AVERROR_EXPERIMENTAL=%d\n",AVERROR_EXPERIMENTAL);
    ALOGI( "AVERROR_INPUT_CHANGED=%d\n",AVERROR_INPUT_CHANGED);
    ALOGI( "AVERROR_OUTPUT_CHANGED=%d\n",AVERROR_OUTPUT_CHANGED);
/* HTTP & RTSP errors */
    ALOGI( "AVERROR_HTTP_BAD_REQUEST=%d\n",AVERROR_HTTP_BAD_REQUEST);
    ALOGI( "AVERROR_HTTP_UNAUTHORIZED=%d\n",AVERROR_HTTP_UNAUTHORIZED);
    ALOGI( "AVERROR_HTTP_FORBIDDEN=%d\n",AVERROR_HTTP_FORBIDDEN);
    ALOGI( "AVERROR_HTTP_NOT_FOUND=%d\n",AVERROR_HTTP_NOT_FOUND);
    ALOGI( "AVERROR_HTTP_OTHER_4XX=%d\n",AVERROR_HTTP_OTHER_4XX);
    ALOGI( "AVERROR_HTTP_SERVER_ERROR=%d\n",AVERROR_HTTP_SERVER_ERROR);
}

//#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)
#define FFMPEG_PLAYER_MAX_QUEUE_SIZE 25
static YtxMediaPlayer* sPlayer;

//FrameQueue *frameQueueVideo;
//FrameQueue *frameQueueAudio;

YtxMediaPlayer::YtxMediaPlayer(){
    wanted_stream_spec[AVMEDIA_TYPE_VIDEO] = "vst";
    wanted_stream_spec[AVMEDIA_TYPE_AUDIO] = "ast";
    wanted_stream_spec[AVMEDIA_TYPE_SUBTITLE] = "sst";

    mListener = NULL;
    mCookie = NULL;
    mDuration = -1;
    mStreamType = -1;
    mCurrentPosition = -1;
    mSeekPosition = -1;
    mCurrentState = MEDIA_PLAYER_IDLE;
    mPrepareSync = false;
    mPrepareStatus = 0;
    abortRequest = 0;
    mLoop = false;
    pthread_mutex_init(&mLock, NULL);
    mLeftVolume = mRightVolume = 1.0;
    mVideoWidth = mVideoHeight = 0;
    memset(st_index, -1, sizeof(st_index));
    mVideoStateInfo = new VideoStateInfo();
    mVideoStateInfo->mCurrentState = &mCurrentState;
    mPlayerPrepareAsync = new PlayerPrepareAsync();
    mMessageLoop = new MessageLoop();
    sPlayer = this;
}

YtxMediaPlayer::~YtxMediaPlayer() {


    if(!mDecoderVideo){
        delete mDecoderVideo;
    }

    if(!mDecoderAudio){
        delete mDecoderAudio;
    }


    delete mVideoStateInfo;
    delete mPlayerPrepareAsync;

    if(!mVideoRefreshController){
        delete mVideoRefreshController;
    }

    if(!audioEngine){
        delete audioEngine;
    }

    if(!mMessageLoop){
        delete mMessageLoop;
    }



}

void YtxMediaPlayer::died() {

}


void  YtxMediaPlayer::disconnect() {

}

int  YtxMediaPlayer::setDataSource(const char *url) {

    this->filePath = url;
    ALOGI("YtxMediaPlayer setDataSource filePath=%s\n",filePath);
    mCurrentState = MEDIA_PLAYER_INITIALIZED;
    return 0;
}

int  YtxMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {

    return 0;
}

int audioDecodeFrame(){

    Frame *af;
    // do{
    if (!(af = sPlayer->mVideoStateInfo->frameQueueAudio->frameQueuePeekReadable())){
        return -1;
    }

    sPlayer->mVideoStateInfo->frameQueueAudio->frameQueueNext();

    // }while(af->serial != is->audioq.serial);

    swr_convert(sPlayer->swrCtx, &(sPlayer->out_buffer_audio), MAX_AUDIO_FRME_SIZE, (const uint8_t **) af->frame->data, af->frame->nb_samples);
    //获取sample的size
    int out_buffer_size = av_samples_get_buffer_size(NULL, sPlayer->out_channel_nb,
                                                     af->frame->nb_samples, sPlayer->out_sample_fmt, 1);

    (*sPlayer->audioEngine->bqPlayerBufferQueue)->Enqueue(sPlayer->audioEngine->bqPlayerBufferQueue, sPlayer->out_buffer_audio, out_buffer_size);
  //  fwrite(sPlayer->out_buffer_audio,1,out_buffer_size,sPlayer->fp_pcm);

    return 0;

}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == sPlayer->audioEngine->bqPlayerBufferQueue);
    assert(NULL == context);
#if 0

    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (--sPlayer->audioEngine->nextCount > 0 && NULL != sPlayer->audioEngine->nextBuffer && 0 != sPlayer->audioEngine->nextSize) {
        SLresult result;
        // enqueue another buffer
        result = (*sPlayer->audioEngine->bqPlayerBufferQueue)->Enqueue(sPlayer->audioEngine->bqPlayerBufferQueue,
                                                                       sPlayer->audioEngine->nextBuffer,
                                                                       sPlayer->audioEngine->nextSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            pthread_mutex_unlock(&audioEngineLock);
//        }
        (void)result;
    }
#else
    audioDecodeFrame();
    if(*sPlayer->mVideoStateInfo->mCurrentState == MEDIA_PLAYER_PAUSED){
        sPlayer->mVideoStateInfo->waitOnNotify(MEDIA_PLAYER_PAUSED);

    }

#endif
}


int  YtxMediaPlayer::prepare() {

    mCurrentState = MEDIA_PLAYER_PREPARING;

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    ALOGI("prepare this->filePath=%s\n",this->filePath);
 //   ALOGI("Couldn't open input stream.\n");
    if(avformat_open_input(&pFormatCtx,this->filePath,NULL,NULL)!=0){
        ALOGI("Couldn't open input stream.\n");
        return -1;
    }

    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        ALOGI("Couldn't find stream information.\n");
        return -1;
    }


    for(int i=0; i<pFormatCtx->nb_streams; i++) {
        AVStream *st = pFormatCtx->streams[i];
        enum AVMediaType type = st->codecpar->codec_type;

        if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1) {
            if (avformat_match_stream_specifier(pFormatCtx, st, wanted_stream_spec[type]) > 0) {
                st_index[type] = i;
            }
        }

    }

    for (int i = 0; i < AVMEDIA_TYPE_NB; i++) {
        if (wanted_stream_spec[i] && st_index[i] == -1) {
            ALOGI("Stream specifier %s does not match any %s stream\n", wanted_stream_spec[(AVMediaType)i], av_get_media_type_string((AVMediaType)i));
            st_index[i] = INT_MAX;
        }
    }


    mVideoStateInfo->pFormatCtx = pFormatCtx;
    if(st_index[AVMEDIA_TYPE_AUDIO] >= 0){
        streamComponentOpen(mVideoStateInfo->streamAudio,st_index[AVMEDIA_TYPE_AUDIO]);
        audioEngine = new AudioEngine();
        audioEngine->createEngine();
        audioEngine->createBufferQueueAudioPlayer(mVideoStateInfo->streamAudio->dec_ctx->sample_rate,960,out_channel_nb,bqPlayerCallback);
    }

    if(st_index[AVMEDIA_TYPE_VIDEO] >= 0){
        streamComponentOpen(mVideoStateInfo->streamVideo,st_index[AVMEDIA_TYPE_VIDEO]);
    }


    fp_yuv = fopen("/storage/emulated/0/output.yuv","wb+");
    fp_pcm = fopen("/storage/emulated/0/output.pcm","wb+");
    ALOGI("start fp_yuv=%d\n",fp_yuv);


    mVideoStateInfo->frameQueueVideo->frameQueueInit(VIDEO_PICTURE_QUEUE_SIZE,1);


    mVideoStateInfo->frameQueueAudio->frameQueueInit(SAMPLE_QUEUE_SIZE,1);

    mDecoderAudio = new DecoderAudio(mVideoStateInfo);
    mDecoderVideo = new DecoderVideo(mVideoStateInfo);


    mDecoderAudio->onDecode = decodeAudio;
    mDecoderVideo->onDecode = decodeVideo;
    mDecoderVideo->onDecodeFinish = finish;
    mDecoderAudio->firstFrameHandler = decodeAudioFirstFrameHandler;

    mVideoRefreshController = new VideoRefreshController(mVideoStateInfo);


    mCurrentState = MEDIA_PLAYER_PREPARED;
    ALOGI("YtxMediaPlayer::prepare OUT\n");
    return 0;

}




int  YtxMediaPlayer::prepareAsync() {

    //mPlayerPrepareAsync->startAsync();
    mCurrentState = MEDIA_PLAYER_PREPARING;
    pthread_create(&mPlayerPrepareAsyncThread, NULL, prepareAsyncPlayer, NULL);
    return 0;
}

void* YtxMediaPlayer::prepareAsyncPlayer(void* ptr){


    av_register_all();
    avformat_network_init();
    sPlayer->pFormatCtx = avformat_alloc_context();

    ALOGI("prepareAsyncPlayer prepare this->filePath=%s\n",sPlayer->filePath);
    //   ALOGI("Couldn't open input stream.\n");
    if(avformat_open_input(&sPlayer->pFormatCtx,sPlayer->filePath,NULL,NULL)!=0){
        ALOGI("Couldn't open input stream.\n");
        //return -1;
    }

    if(avformat_find_stream_info(sPlayer->pFormatCtx,NULL)<0){
        ALOGI("Couldn't find stream information.\n");
       // return -1;
    }


    for(int i=0; i<sPlayer->pFormatCtx->nb_streams; i++) {
        AVStream *st = sPlayer->pFormatCtx->streams[i];
        enum AVMediaType type = st->codecpar->codec_type;

        if (type >= 0 && sPlayer->wanted_stream_spec[type] && sPlayer->st_index[type] == -1) {
            if (avformat_match_stream_specifier(sPlayer->pFormatCtx, st, sPlayer->wanted_stream_spec[type]) > 0) {
                sPlayer->st_index[type] = i;
            }
        }

    }

    for (int i = 0; i < AVMEDIA_TYPE_NB; i++) {
        if (sPlayer->wanted_stream_spec[i] && sPlayer->st_index[i] == -1) {
            ALOGI("Stream specifier %s does not match any %s stream\n", sPlayer->wanted_stream_spec[(AVMediaType)i], av_get_media_type_string((AVMediaType)i));
            sPlayer->st_index[i] = INT_MAX;
        }
    }


    sPlayer->mVideoStateInfo->pFormatCtx = sPlayer->pFormatCtx;
    if(sPlayer->st_index[AVMEDIA_TYPE_AUDIO] >= 0){
        sPlayer->streamComponentOpen(sPlayer->mVideoStateInfo->streamAudio,sPlayer->st_index[AVMEDIA_TYPE_AUDIO]);
        sPlayer->audioEngine = new AudioEngine();
        sPlayer->audioEngine->createEngine();
        sPlayer->audioEngine->createBufferQueueAudioPlayer(sPlayer->mVideoStateInfo->streamAudio->dec_ctx->sample_rate,960,sPlayer->out_channel_nb,bqPlayerCallback);
    }

    if(sPlayer->st_index[AVMEDIA_TYPE_VIDEO] >= 0){
        sPlayer->streamComponentOpen(sPlayer->mVideoStateInfo->streamVideo,sPlayer->st_index[AVMEDIA_TYPE_VIDEO]);
    }

    sPlayer->fp_yuv = fopen("/storage/emulated/0/output.yuv","wb+");
    sPlayer->fp_pcm = fopen("/storage/emulated/0/output.pcm","wb+");
    ALOGI("start fp_yuv=%d\n",sPlayer->fp_yuv);


    sPlayer->mVideoStateInfo->frameQueueVideo->frameQueueInit(VIDEO_PICTURE_QUEUE_SIZE,1);


    sPlayer->mVideoStateInfo->frameQueueAudio->frameQueueInit(SAMPLE_QUEUE_SIZE,1);

    sPlayer->mDecoderAudio = new DecoderAudio(sPlayer->mVideoStateInfo);
    sPlayer->mDecoderVideo = new DecoderVideo(sPlayer->mVideoStateInfo);


    sPlayer->mDecoderAudio->onDecode = decodeAudio;
    sPlayer->mDecoderVideo->onDecode = decodeVideo;
    sPlayer->mDecoderVideo->onDecodeFinish = finish;
    sPlayer->mDecoderAudio->firstFrameHandler = decodeAudioFirstFrameHandler;

    sPlayer->mVideoRefreshController = new VideoRefreshController(sPlayer->mVideoStateInfo);


    AVMessage msg;
    msg.what = FFP_MSG_PREPARED;
    sPlayer->mMessageLoop->enqueue(&msg);
    sPlayer->mCurrentState = MEDIA_PLAYER_PREPARED;
    pthread_exit(NULL);
}

int YtxMediaPlayer::resume() {
    mCurrentState = MEDIA_PLAYER_STARTED;
    mVideoStateInfo->notifyAll();
    return 0;
}

int  YtxMediaPlayer::start() {
    ALOGI("YtxMediaPlayer::start() mCurrentState=%d \n",mCurrentState);

    if(mCurrentState == MEDIA_PLAYER_PAUSED){
        return  resume();

    }

//    if(mCurrentState == MEDIA_PLAYBACK_COMPLETE){
//        goto START_PLAYER;
//    }
//
//    if(mCurrentState != MEDIA_PLAYER_PREPARED){
//        return -4;
//    }
//    START_PLAYER:
    pthread_create(&mPlayerThread, NULL, startPlayer, NULL);

    return 0;
}

void* YtxMediaPlayer::startPlayer(void* ptr)
{
    ALOGI("starting main player thread\n");
    printferr();
    //等待surface render初始化完成

    do{
        usleep(200);
    }while(!GlEngine::glEngineInitComplete());
  //  GlEngine::getGlEngine()->setVideoWidthAndHeight(sPlayer->streamVideo.dec_ctx->width,sPlayer->streamVideo.dec_ctx->height);

    sPlayer->decodeMovie(ptr);
    return 0;
}

void YtxMediaPlayer::decodeAudioFirstFrameHandler(){
    audioDecodeFrame();
}


void YtxMediaPlayer::notifyRenderer() {
    //调用回调通知渲染视频
    if(notifyRendererCallback != NULL){
        notifyRendererCallback();
    }

}

void YtxMediaPlayer::checkSeekRequest() {

    int ret;
    if(mVideoStateInfo != NULL){
        if(mVideoStateInfo->seekReq){
            int64_t seek_target = mVideoStateInfo->seekPos;
            int64_t seek_min    = mVideoStateInfo->seekRel > 0 ? seek_target - mVideoStateInfo->seekRel + 2: INT64_MIN;
            int64_t seek_max    = mVideoStateInfo->seekRel < 0 ? seek_target - mVideoStateInfo->seekRel - 2: INT64_MAX;
            ALOGI("checkSeekRequest seek_target=%lld seek_min=%lld seek_max=%lld mVideoStateInfo->seekFlags=%d\n",seek_target,seek_min,seek_max,mVideoStateInfo->seekFlags);
            ret = avformat_seek_file(mVideoStateInfo->pFormatCtx, -1, seek_min, seek_target, seek_max, mVideoStateInfo->seekFlags);
            ALOGI("while seeking ret = %d\n",ret);
            if(ret<0){
                ALOGI("%s: error while seeking\n",filePath);
            }else {
//                if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
//                    mDecoderAudio->flush();
//                    mDecoderAudio->enqueue(mVideoStateInfo->flushPkt);
//                }

                if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
                    mDecoderVideo->flush();
                    ALOGI("12mVideoStateInfo->flushPkt->pkt.data = %#x \n",mVideoStateInfo->flushPkt->pkt.data);
                    mDecoderVideo->enqueue(mVideoStateInfo->flushPkt);
                }
                if (mVideoStateInfo->seekFlags & AVSEEK_FLAG_BYTE) {
                   // set_clock(&is->extclk, NAN, 0);
                } else {
                   // set_clock(&is->extclk, seek_target / (double)AV_TIME_BASE, 0);
                    mVideoStateInfo->setClock(mVideoStateInfo->extClk,seek_target / (double)AV_TIME_BASE,0);
                }
            }

            mVideoStateInfo->seekReq = false;
            mVideoStateInfo->eof = 0;
        }
    }


}


void YtxMediaPlayer::decodeMovie(void* ptr)
{
   // AVPacket mPacket, *pPacket = &mPacket;

    MAVPacket mPacket, *pPacket = &mPacket;

    mVideoRefreshController->startAsync();

    mDecoderAudio->startAsync();




    mDecoderVideo->startAsync();

    mCurrentState = MEDIA_PLAYER_STARTED;
    ALOGI("playing %ix%i\n", mVideoWidth, mVideoHeight);

    while (mCurrentState != MEDIA_PLAYER_DECODED && mCurrentState != MEDIA_PLAYER_STOPPED &&
           mCurrentState != MEDIA_PLAYER_STATE_ERROR )
    {

        if (mDecoderVideo->packets() > FFMPEG_PLAYER_MAX_QUEUE_SIZE ||
            mDecoderAudio->packets() > FFMPEG_PLAYER_MAX_QUEUE_SIZE) {
           // ALOGI("decodeMovie ret usleep(20) in\n");
            usleep(25);
          //  ALOGI("decodeMovie ret usleep(20) out\n");
            continue;
        }

        ALOGI("decodeMovie ret checkSeekRequest int\n");
        checkSeekRequest();

      //  ALOGI("decodeMovie ret checkSeekRequest out\n");
        ALOGI("decodeMovie ret in\n");
        int ret = av_read_frame(pFormatCtx, &pPacket->pkt);
        ALOGI("decodeMovie ret out=%d\n",ret);

        if(ret < 0) {

            if(ret == AVERROR_EOF || avio_feof(mVideoStateInfo->pFormatCtx->pb) && !mVideoStateInfo->eof){

                if(st_index[AVMEDIA_TYPE_VIDEO]>=0){
                   // packet_queue_put_nullpacket(&is->videoq, is->video_stream);
                  //  mDecoderVideo->enqueueNullPacket(st_index[AVMEDIA_TYPE_VIDEO]);
                }
                mVideoStateInfo->eof = 1;

            }


//            if (mVideoStateInfo->pFormatCtx->pb && mVideoStateInfo->pFormatCtx->pb->error)
//                break;

//            struct timeval now;
//            struct timespec outTime;
//            gettimeofday(&now, NULL);
//            pthread_mutex_lock(&mVideoStateInfo->wait_mutex);
//
//            outTime.tv_nsec =
//            pthread_cond_timedwait(&mVideoStateInfo->continue_read_thread,&mVideoStateInfo->wait_mutex,&outTime);
//            pthread_mutex_unlock(&mVideoStateInfo->wait_mutex);


//            mCurrentState = MEDIA_PLAYER_DECODED;
//
//            pPacket->isEnd = false;
//            mDecoderVideo->enqueue(pPacket);
//            mDecoderAudio->enqueue(pPacket);

            continue;
        }else{
            mVideoStateInfo->eof = 0;
        }

        if (pPacket->pkt.stream_index == st_index[AVMEDIA_TYPE_VIDEO]) {
            ALOGI("decodeMovie ret mDecoderVideo->enqueue(pPacket)=%d ",pPacket);
            mDecoderVideo->enqueue(pPacket);

        }else if(pPacket->pkt.stream_index == st_index[AVMEDIA_TYPE_AUDIO] && false){
            mDecoderAudio->enqueue(pPacket);
        } else {
            av_packet_unref(&pPacket->pkt);
        }
    }

    //waits on end of video thread
    ALOGI("waiting on video thread\n");
    int ret = -1;
    if((ret = mDecoderVideo->wait()) != 0) {
        ALOGI("Couldn't cancel video thread: %i", ret);
    }

    ALOGI("waiting on audio thread");
    if((ret = mDecoderAudio->wait()) != 0) {
        ALOGI( "Couldn't cancel audio thread: %i", ret);
    }


    if(mCurrentState == MEDIA_PLAYER_STATE_ERROR) {
        ALOGI( "playing err\n");
    }
    mCurrentState = MEDIA_PLAYER_PLAYBACK_COMPLETE;
    ALOGI( "end of playing\n");
    fclose(fp_yuv);
    fclose(fp_pcm);
}

void YtxMediaPlayer::decodeAudio(AVFrame* frame, double pts)
{

    ALOGI("decode audio IN");
    swr_convert(sPlayer->swrCtx, &(sPlayer->out_buffer_audio), MAX_AUDIO_FRME_SIZE, (const uint8_t **) frame->data, frame->nb_samples);
    //获取sample的size
    int out_buffer_size = av_samples_get_buffer_size(NULL, sPlayer->out_channel_nb,
                                                     frame->nb_samples, sPlayer->out_sample_fmt, 1);
    ALOGI("decode audio frame->nb_samples=%d\n",frame->nb_samples);
    fwrite(sPlayer->out_buffer_audio,1,out_buffer_size,sPlayer->fp_pcm);
    ALOGI("decode audio OUT");
}


void YtxMediaPlayer::decodeVideo(AVFrame* frame, double pts)
{
    if(FPS_DEBUGGING) {

    }
}

int  YtxMediaPlayer::stop() {

    mCurrentState = MEDIA_PLAYER_STOPPED;
   // playing = false;
    return 0;
}

int  YtxMediaPlayer::pause() {
    ALOGI("YtxMediaPlayer::pause()");
    mCurrentState = MEDIA_PLAYER_PAUSED;
    return 0;
}

bool YtxMediaPlayer::isPlaying() {

    return mCurrentState == MEDIA_PLAYER_STARTED || mCurrentState == MEDIA_PLAYER_DECODED;
}

int  YtxMediaPlayer::getVideoWidth() {

    return mVideoWidth;
}

int  YtxMediaPlayer::getVideoHeight() {

    return mVideoHeight;
}

int  YtxMediaPlayer::seekTo(int msec) {

//    if (!is->seek_req) {
//        is->seek_pos = pos;
//        is->seek_rel = rel;
//        is->seek_flags &= ~AVSEEK_FLAG_BYTE;
//        if (seek_by_bytes)
//            is->seek_flags |= AVSEEK_FLAG_BYTE;
//        is->seek_req = 1;
//        SDL_CondSignal(is->continue_read_thread);
//    }

    double incr, pos;
    if(msec>0){
        incr = 9;
    }else{
        incr = -9;
    }

    ALOGI("seekTo incr=%lf\n",incr);
    if(!mVideoStateInfo->seekReq){

        pos = mVideoStateInfo->getClock(mVideoStateInfo->vidClk);
        if (isnan(pos)) {
            pos = (double) mVideoStateInfo->seekPos / AV_TIME_BASE;
        }
        pos += incr;

        if (mVideoStateInfo->pFormatCtx->start_time != AV_NOPTS_VALUE && pos < mVideoStateInfo->pFormatCtx->start_time / (double)AV_TIME_BASE){
            pos = mVideoStateInfo->pFormatCtx->start_time / (double)AV_TIME_BASE;
        }

        ALOGI("seekTo pos=%lf\n",pos);
        mVideoStateInfo->seekPos = (int64_t)(pos * AV_TIME_BASE);
        mVideoStateInfo->seekRel = (int64_t)(incr * AV_TIME_BASE);

        mVideoStateInfo->seekReq = true;
    }
    return 0;
}

int  YtxMediaPlayer::getCurrentPosition(int *msec) {

    return 0;
}

int  YtxMediaPlayer::getDuration(int *msec) {

    return 0;
}

int  YtxMediaPlayer::reset() {

    return 0;
}

//    int        setAudioStreamType(audio_stream_type_t type);
int   YtxMediaPlayer::setLooping(int loop) {

    return 0;
}

bool  YtxMediaPlayer::isLooping() {

    return 0;
}

int   YtxMediaPlayer::setVolume(float leftVolume, float rightVolume) {

    return 0;
}

int   YtxMediaPlayer::setAudioSessionId(int sessionId) {

    return 0;
}

int   YtxMediaPlayer::getAudioSessionId() {

    return 0;
}

int   YtxMediaPlayer::setAuxEffectSendLevel(float level) {

    return 0;
}

int   YtxMediaPlayer::attachAuxEffect(int effectId) {

    return 0;
}

int   YtxMediaPlayer::setRetransmitEndpoint(const char *addrString, uint16_t port) {

    return 0;
}


int YtxMediaPlayer::updateProxyConfig(const char *host, int32_t port, const char *exclusionList) {

    return 0;
}

void YtxMediaPlayer::clear_l() {


}

int  YtxMediaPlayer::seekTo_l(int msec) {

    return 0;
}

int  YtxMediaPlayer::prepareAsync_l() {

    return 0;
}

int  YtxMediaPlayer::getDuration_l(int *msec) {

    return 0;
}

int  YtxMediaPlayer::reset_l() {

    return 0;
}

int  YtxMediaPlayer::setListener(MediaPlayerListener* listener) {

    this->mListener = listener;
    this->mMessageLoop->setMsgListener(mListener);
    mMessageLoop->startAsync();
    return 0;
}

void  YtxMediaPlayer::finish() {

    ALOGI("YtxMediaPlayer::finish IN");
    sPlayer->mVideoRefreshController->stop();
    sPlayer->mMessageLoop->stop();
    sPlayer->isFinish = 1;
    ALOGI("YtxMediaPlayer::finish OUT");
}


int YtxMediaPlayer::streamComponentOpen(InputStream *is, int stream_index)
{

    if (stream_index < 0 || stream_index > pFormatCtx->nb_streams) {
        return -1;
    }


    is->dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(is->dec_ctx, pFormatCtx->streams[stream_index]->codecpar);

    av_codec_set_pkt_timebase(is->dec_ctx, pFormatCtx->streams[stream_index]->time_base);
    is->st = pFormatCtx->streams[stream_index];
    AVCodec* codec = avcodec_find_decoder(is->dec_ctx->codec_id);
    if (codec == NULL) {
        return -1;
    }

    ALOGI("streamVideo.dec_ctx->codec_id=%d\n",is->dec_ctx->codec_id);

    is->dec_ctx->codec_id = codec->id;



    // Open codec
    if (avcodec_open2(is->dec_ctx, codec,NULL) < 0) {
        return -1;
    }


    switch (is->dec_ctx->codec_type){
        case AVMEDIA_TYPE_AUDIO:
            {
                //解压缩数据
                mFrameAudio = av_frame_alloc();
                //frame->16bit 44100 PCM 统一音频采样格式与采样率
                swrCtx = swr_alloc();
                //重采样设置参数-------------start
                //输入的采样格式
                in_sample_fmt = is->dec_ctx->sample_fmt;
                //输出采样格式16bit PCM
                out_sample_fmt = AV_SAMPLE_FMT_S16;
                //输入采样率
                in_sample_rate = is->dec_ctx->sample_rate;
                //输出采样率
                //out_sample_rate = 44100;
                out_sample_rate = in_sample_rate;
                //获取输入的声道布局
                //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
                //av_get_default_channel_layout(codecCtx->channels);
                out_nb_samples=is->dec_ctx->frame_size;

                uint64_t in_ch_layout = is->dec_ctx->channel_layout;
                //输出的声道布局（立体声）
                uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

                swr_alloc_set_opts(swrCtx,
                                   out_ch_layout,out_sample_fmt,out_sample_rate,
                                   in_ch_layout,in_sample_fmt,in_sample_rate,
                                   0, NULL);

                swr_init(swrCtx);

                //输出的声道个数
                out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

                //重采样设置参数-------------end
                //16bit 44100 PCM 数据
                out_buffer_audio = (uint8_t *)av_malloc(MAX_AUDIO_FRME_SIZE);

                ALOGI("### in_sample_rate=%d\n",in_sample_rate);
                ALOGI("### in_sample_fmt=%d\n",in_sample_fmt);
                ALOGI("### out_nb_samples=%d\n",out_nb_samples);
                ALOGI("### out_sample_rate=%d\n",out_sample_rate);
                ALOGI("### out_sample_fmt=%d\n",out_sample_fmt);
                ALOGI("### out_channel_nb=%d\n",out_channel_nb);

                ALOGI("### in_ch_layout=%d\n",in_ch_layout);
                ALOGI("### out_ch_layout=%d\n",out_ch_layout);
            }
            break;
        case AVMEDIA_TYPE_VIDEO:

            mVideoWidth = is->dec_ctx->width;
            mVideoHeight = is->dec_ctx->height;
            mDuration =  pFormatCtx->duration;

            mConvertCtx = sws_getContext(is->dec_ctx->width,
                                         is->dec_ctx->height,
                                         is->dec_ctx->pix_fmt,
                                         is->dec_ctx->width,
                                         is->dec_ctx->height,
                                         AV_PIX_FMT_YUV420P,
                                         SWS_BICUBIC,
                                         NULL,
                                         NULL,
                                         NULL);

            if (mConvertCtx == NULL) {
                return -1;
            }

            mFrameVideo = av_frame_alloc();
            mYuvFrame = av_frame_alloc();

            if (mFrameVideo == NULL || mYuvFrame ==NULL) {
                return -1;
            }

            out_buffer_video=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  is->dec_ctx->width, is->dec_ctx->height,1));
            av_image_fill_arrays(mYuvFrame->data, mYuvFrame->linesize,out_buffer_video,
                                 AV_PIX_FMT_YUV420P,is->dec_ctx->width, is->dec_ctx->height,1);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            break;
    }


    return 0;
}