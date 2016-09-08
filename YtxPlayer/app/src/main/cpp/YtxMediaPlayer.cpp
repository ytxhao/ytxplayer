//
// Created by Administrator on 2016/9/2.
//


#include "YtxMediaPlayer.h"

static YtxMediaPlayer* sPlayer;


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
    mLoop = false;
    pthread_mutex_init(&mLock, NULL);
    mLeftVolume = mRightVolume = 1.0;
    mVideoWidth = mVideoHeight = 0;
    memset(st_index, -1, sizeof(st_index));
    sPlayer = this;
}

void YtxMediaPlayer::died() {

}


void  YtxMediaPlayer::disconnect() {

}

int  YtxMediaPlayer::setDataSource(const char *url) {

    this->filePath = url;
    ALOGI("YtxMediaPlayer setDataSource filePath=%s",filePath);
    return 0;
}

int  YtxMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {

    return 0;
}

int  YtxMediaPlayer::prepare() {

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
        st->discard = AVDISCARD_ALL;
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

//    mAudioStreamIndex = -1;
//    for(int i=0; i<pFormatCtx->nb_streams; i++) {
////        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
////            mVideoStreamIndex = i;
////            break;
////        }
//
//        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            mAudioStreamIndex = i;
//            break;
//        }
//
//    }
//
//    if(mAudioStreamIndex == -1){
//        ALOGI("Didn't find a audio stream.\n");
//        return -1;
//    }
//
//
//    if(mVideoStreamIndex==-1){
//        ALOGI("Didn't find a video stream.\n");
//        return -1;
//    }


    if (st_index[AVMEDIA_TYPE_VIDEO] < 0 || st_index[AVMEDIA_TYPE_VIDEO] > pFormatCtx->nb_streams) {
        return -1;
    }


   // AVStream* stream = pFormatCtx->streams[mVideoStreamIndex];
    // Get a pointer to the codec context for the video stream
    AVCodecContext* codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->codecpar);

    av_codec_set_pkt_timebase(codec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->time_base);

    streamVideo.dec_ctx = codec_ctx;
    streamVideo.st = pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]];
    //AVCodecContext* codec_ctx = stream->codecpar
    AVCodec* codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (codec == NULL) {
        return -1;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec,NULL) < 0) {
        return -1;
    }

    mVideoWidth = codec_ctx->width;
    mVideoHeight = codec_ctx->height;
    mDuration =  pFormatCtx->duration;

    mConvertCtx = sws_getContext(codec_ctx->width,
                                 codec_ctx->height,
                                 codec_ctx->pix_fmt,
                                 codec_ctx->width,
                                 codec_ctx->height,
                                 AV_PIX_FMT_YUV420P,
                                 SWS_BICUBIC,
                                 NULL,
                                 NULL,
                                 NULL);

    if (mConvertCtx == NULL) {
        return -1;
    }

    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return -1;
    }

    ALOGI("YtxMediaPlayer::prepare OUT");
    return 0;




}

int  YtxMediaPlayer::prepareAsync() {

    return 0;
}

int  YtxMediaPlayer::start() {


    pthread_create(&mPlayerThread, NULL, startPlayer, NULL);

    return 0;
}

void* YtxMediaPlayer::startPlayer(void* ptr)
{
    ALOGI("starting main player thread");
    sPlayer->decodeMovie(ptr);
    return 0;
}


void YtxMediaPlayer::decodeMovie(void* ptr)
{
    AVPacket pPacket;

  //  AVStream* stream_audio = mMovieFile->streams[mAudioStreamIndex];
  //  mDecoderAudio = new DecoderAudio(stream_audio);
  //  mDecoderAudio->onDecode = decode;
 //   mDecoderAudio->startAsync();

   // AVStream* stream_video = pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]];


    mDecoderVideo = new DecoderVideo(&streamVideo);
    mDecoderVideo->onDecode = decode;
    mDecoderVideo->startAsync();

    mCurrentState = MEDIA_PLAYER_STARTED;
    ALOGI("playing %ix%i", mVideoWidth, mVideoHeight);
    while (mCurrentState != MEDIA_PLAYER_DECODED && mCurrentState != MEDIA_PLAYER_STOPPED &&
           mCurrentState != MEDIA_PLAYER_STATE_ERROR)
    {
//        if (mDecoderVideo->packets() > FFMPEG_PLAYER_MAX_QUEUE_SIZE &&
//            mDecoderAudio->packets() > FFMPEG_PLAYER_MAX_QUEUE_SIZE) {
//            usleep(200);
//            continue;
//        }

        if(av_read_frame(pFormatCtx, &pPacket) < 0) {
            mCurrentState = MEDIA_PLAYER_DECODED;
            continue;
        }

        // Is this a packet from the video stream?
        if (pPacket.stream_index == mVideoStreamIndex) {
            mDecoderVideo->enqueue(&pPacket);
        }
//
//        else if (pPacket.stream_index == mAudioStreamIndex) {
//            mDecoderAudio->enqueue(&pPacket);
//        }
//        else {
//            // Free the packet that was allocated by av_read_frame
//            av_free_packet(&pPacket);
//        }
    }

    //waits on end of video thread
    ALOGI("waiting on video thread");
    int ret = -1;
    if((ret = mDecoderVideo->wait()) != 0) {
        ALOGI("Couldn't cancel video thread: %i", ret);
    }

//    ALOGI("waiting on audio thread");
//    if((ret = mDecoderAudio->wait()) != 0) {
//        ALOGI( "Couldn't cancel audio thread: %i", ret);
//    }

    if(mCurrentState == MEDIA_PLAYER_STATE_ERROR) {
        ALOGI( "playing err");
    }
    mCurrentState = MEDIA_PLAYER_PLAYBACK_COMPLETE;
    ALOGI( "end of playing");
}


void YtxMediaPlayer::decode(AVFrame* frame, double pts)
{
    if(false) {
        timeval pTime;
        static int frames = 0;
        static double t1 = -1;
        static double t2 = -1;

        gettimeofday(&pTime, NULL);
        t2 = pTime.tv_sec + (pTime.tv_usec / 1000000.0);
        if (t1 == -1 || t2 > t1 + 1) {
            ALOGI("Video fps:%i", frames);
            //sPlayer->notify(MEDIA_INFO_FRAMERATE_VIDEO, frames, -1);
            t1 = t2;
            frames = 0;
        }
        frames++;
    }

    // Convert the image from its native format to RGB
//    sws_scale(mConvertCtx,
//              (const uint8_t *const *) frame->data,
//              frame->linesize,
//              0,
//              mVideoHeight,
//              mFrame->data,
//              mFrame->linesize);

  //  Output::VideoDriver_updateSurface();
}

int  YtxMediaPlayer::stop() {

    return 0;
}

int  YtxMediaPlayer::pause() {

    return 0;
}

bool YtxMediaPlayer::isPlaying() {

    return 0;
}

int  YtxMediaPlayer::getVideoWidth(int *w) {

    return 0;
}

int  YtxMediaPlayer::getVideoHeight(int *h) {

    return 0;
}

int  YtxMediaPlayer::seekTo(int msec) {

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

int  YtxMediaPlayer::setListener(const MediaPlayerListener* listener) {

    return 0;
}