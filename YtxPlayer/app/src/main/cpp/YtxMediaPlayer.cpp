//
// Created by Administrator on 2016/9/2.
//



#include "YtxMediaPlayer.h"

#define FPS_DEBUGGING true

void printfferr(){
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
    ALOGI("YtxMediaPlayer setDataSource filePath=%s\n",filePath);
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
       // st->discard = AVDISCARD_ALL;
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


    if (st_index[AVMEDIA_TYPE_VIDEO] < 0 || st_index[AVMEDIA_TYPE_VIDEO] > pFormatCtx->nb_streams) {
        return -1;
    }


   // AVStream* stream = pFormatCtx->streams[mVideoStreamIndex];
    // Get a pointer to the codec context for the video stream
   // AVCodecContext* codec_ctx = avcodec_alloc_context3(NULL);
    streamVideo.dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(streamVideo.dec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->codecpar);

    av_codec_set_pkt_timebase(streamVideo.dec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->time_base);

  //  streamVideo.dec_ctx = codec_ctx;
    streamVideo.st = pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]];
    //AVCodecContext* codec_ctx = stream->codecpar
    AVCodec* codec = avcodec_find_decoder(streamVideo.dec_ctx->codec_id);
    if (codec == NULL) {
        return -1;
    }

    printf("streamVideo.dec_ctx->codec_id=%d\n",streamVideo.dec_ctx->codec_id);

    streamVideo.dec_ctx->codec_id = codec->id;

    // Open codec
    if (avcodec_open2(streamVideo.dec_ctx, codec,NULL) < 0) {
        return -1;
    }

    mVideoWidth = streamVideo.dec_ctx->width;
    mVideoHeight = streamVideo.dec_ctx->height;
    mDuration =  pFormatCtx->duration;

    mConvertCtx = sws_getContext(streamVideo.dec_ctx->width,
                                 streamVideo.dec_ctx->height,
                                 streamVideo.dec_ctx->pix_fmt,
                                 streamVideo.dec_ctx->width,
                                 streamVideo.dec_ctx->height,
                                 AV_PIX_FMT_YUV420P,
                                 SWS_BICUBIC,
                                 NULL,
                                 NULL,
                                 NULL);

    if (mConvertCtx == NULL) {
        return -1;
    }

    mFrame = av_frame_alloc();
    mYuvFrame = av_frame_alloc();

    if (mFrame == NULL || mYuvFrame ==NULL) {
        return -1;
    }

    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  streamVideo.dec_ctx->width, streamVideo.dec_ctx->height,1));
    av_image_fill_arrays(mYuvFrame->data, mYuvFrame->linesize,out_buffer,
                         AV_PIX_FMT_YUV420P,streamVideo.dec_ctx->width, streamVideo.dec_ctx->height,1);
    ALOGI("YtxMediaPlayer::prepare OUT\n");
    return 0;




}

int  YtxMediaPlayer::prepareAsync() {

    return 0;
}

int  YtxMediaPlayer::start() {

    fp_yuv = fopen("/storage/emulated/0/output.yuv","wb+");
    char datam[1024]="ytxh ytxhaooooolkaslfasl;'kf'as ";
   // fwrite(datam,1,1024,fp_yuv);
    ALOGI("start fp_yuv=%d\n",fp_yuv);
    ALOGI("&streamVideo=%d ; streamVideo.dec_ctx=%d\n",&streamVideo,streamVideo.dec_ctx);
    pthread_create(&mPlayerThread, NULL, startPlayer, NULL);

    return 0;
}

void* YtxMediaPlayer::startPlayer(void* ptr)
{
    ALOGI("starting main player thread\n");
  //  printfferr();
    sPlayer->decodeMovie(ptr);
    return 0;
}




void YtxMediaPlayer::decodeMovie(void* ptr)
{
    AVPacket mPacket, *pPacket = &mPacket;
    int i=0, *pI = &i;
  //  AVStream* stream_audio = mMovieFile->streams[mAudioStreamIndex];
  //  mDecoderAudio = new DecoderAudio(stream_audio);
  //  mDecoderAudio->onDecode = decode;
 //   mDecoderAudio->startAsync();

   // AVStream* stream_video = pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]];


    mDecoderVideo = new DecoderVideo(&streamVideo);
    mDecoderVideo->onDecode = decode;
    mDecoderVideo->onDecodeFinish = finish;
    mDecoderVideo->startAsync();

    mCurrentState = MEDIA_PLAYER_STARTED;
    ALOGI("playing %ix%i\n", mVideoWidth, mVideoHeight);

    while (mCurrentState != MEDIA_PLAYER_DECODED && mCurrentState != MEDIA_PLAYER_STOPPED &&
           mCurrentState != MEDIA_PLAYER_STATE_ERROR )
    {

        usleep(400);

        int ret = av_read_frame(pFormatCtx, pPacket);
        ALOGI("decodeMovie ret=%d",ret);
        if(ret < 0) {
            mCurrentState = MEDIA_PLAYER_DECODED;
            *pI = 1;
            mDecoderVideo->enqueue(pPacket,pI);
          //  mDecoderVideo->stop();
          //  mDecoderVideo->isFinish = 1;
            continue;
        }

        if (pPacket->stream_index == st_index[AVMEDIA_TYPE_VIDEO]) {
            ALOGI("mDecoderVideo->enqueue(pPacket)=%d  *pI=%d",pPacket,*pI);
            mDecoderVideo->enqueue(pPacket,pI);
           // mDecoderVideo->isFinish = 0;
        } else {
            av_packet_unref(pPacket);
        }
    }

    //waits on end of video thread
    ALOGI("waiting on video thread\n");
    int ret = -1;
    if((ret = mDecoderVideo->wait()) != 0) {
        ALOGI("Couldn't cancel video thread: %i", ret);
    }

//    ALOGI("waiting on audio thread");
//    if((ret = mDecoderAudio->wait()) != 0) {
//        ALOGI( "Couldn't cancel audio thread: %i", ret);
//    }

    if(mCurrentState == MEDIA_PLAYER_STATE_ERROR) {
        ALOGI( "playing err\n");
    }
    mCurrentState = MEDIA_PLAYER_PLAYBACK_COMPLETE;
    ALOGI( "end of playing\n");
    fclose(fp_yuv);
}




void YtxMediaPlayer::decode(AVFrame* frame, double pts)
{
    if(FPS_DEBUGGING) {
        timeval pTime;
        static int frames = 0;
        static double t1 = -1;
        static double t2 = -1;

        gettimeofday(&pTime, NULL);
        t2 = pTime.tv_sec + (pTime.tv_usec / 1000000.0);
        if (t1 == -1 || t2 > t1 + 1) {
            ALOGI("Video fps:%i\n", frames);
            //sPlayer->notify(MEDIA_INFO_FRAMERATE_VIDEO, frames, -1);
            t1 = t2;
            frames = 0;
        }
        frames++;
    }

  //  ALOGI("decode frame %d; data[0]=%d\n",frame->data,frame->data[0]);
    // Convert the image from its native format to RGB
    sws_scale(sPlayer->mConvertCtx,
              (const unsigned char *const *) frame->data,
              frame->linesize,
              0,
              sPlayer->mVideoHeight,
              sPlayer->mYuvFrame->data,
              sPlayer->mYuvFrame->linesize);

 //   ALOGI("decode mYuvFrame %d; mYuvFrame[0]=%d;sPlayer->fp_yuv=%d",sPlayer->mYuvFrame->data,sPlayer->mYuvFrame->data[0],sPlayer->fp_yuv);


    int y_size= sPlayer->streamVideo.dec_ctx->width*sPlayer->streamVideo.dec_ctx->height;
  //  ALOGI("y_size=%d ; sPlayer->fp_yuv=%d\n",y_size,sPlayer->fp_yuv);
    fwrite(sPlayer->mYuvFrame->data[0],1,y_size,sPlayer->fp_yuv);    //Y
    fwrite(sPlayer->mYuvFrame->data[1],1,y_size/4,sPlayer->fp_yuv);  //U
    fwrite(sPlayer->mYuvFrame->data[2],1,y_size/4,sPlayer->fp_yuv);  //V
    // Output::VideoDriver_updateSurface();
    ALOGI("y_size=%d ; sPlayer->mYuvFrame->data[0]=%d\n",y_size,sPlayer->mYuvFrame->data[0]);
    sPlayer->bindTexture(sPlayer->mYuvFrame->data[0],sPlayer->mYuvFrame->data[1],sPlayer->mYuvFrame->data[2]);
    sPlayer->updateYuv(sPlayer->mYuvFrame->data[0],sPlayer->mYuvFrame->data[1],sPlayer->mYuvFrame->data[2],y_size);
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

void  YtxMediaPlayer::finish() {

    ALOGI("YtxMediaPlayer::finish");
 //   fclose(sPlayer->fp_yuv);
  //  return ;
}

void YtxMediaPlayer::setTexture(int textureY,int textureU,int textureV)
{
    this->textureY = textureY;
    this->textureU = textureU;
    this->textureV = textureV;
}

void YtxMediaPlayer::bindTexture(uint8_t *y,uint8_t *u,uint8_t *v) {


    // building texture for Y data
//    if (_ytid < 0 || videoSizeChanged) {
//        if (_ytid >= 0) {
//            YtxLog.d("GLProgram","glDeleteTextures Y");
//            GLES20.glDeleteTextures(1, new int[] { _ytid }, 0);
//            checkGlError("glDeleteTextures");
//        }
//        // GLES20.glPixelStorei(GLES20.GL_UNPACK_ALIGNMENT, 1);
//        int[] textures = new int[1];
//            GLES20.glGenTextures(1, textures, 0);
//            checkGlError("glGenTextures");
//            _ytid = textures[0];
//            YtxLog.d("GLProgram","glGenTextures Y = " + _ytid);
//        }
//        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _ytid);
//        checkGlError("glBindTexture");
//        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, _video_width, _video_height, 0,
//                GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);
//        checkGlError("glTexImage2D");
//        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
//        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
//        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
//        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);


    glBindTexture(GL_TEXTURE_2D,textureY);

    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,mVideoWidth,mVideoHeight,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,y);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



    glBindTexture(GL_TEXTURE_2D,textureU);

    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,mVideoWidth/2,mVideoHeight/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,u);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glBindTexture(GL_TEXTURE_2D,textureV);

    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,mVideoWidth/2,mVideoHeight/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,v);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}