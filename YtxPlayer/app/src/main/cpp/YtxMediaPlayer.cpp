//
// Created by Administrator on 2016/9/2.
//




#include "YtxMediaPlayer.h"

#define MAX_AUDIO_FRME_SIZE 48000 * 4
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

//#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

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

    streamVideo.pFormatCtx = pFormatCtx;
    streamAudio.pFormatCtx = pFormatCtx;
    if(st_index[AVMEDIA_TYPE_AUDIO] >= 0){
        streamComponentOpen(&streamAudio,st_index[AVMEDIA_TYPE_AUDIO]);
    }

    if(st_index[AVMEDIA_TYPE_VIDEO] >= 0){
        streamComponentOpen(&streamVideo,st_index[AVMEDIA_TYPE_VIDEO]);
    }


    ALOGI("YtxMediaPlayer::prepare OUT\n");
    return 0;




}

int  YtxMediaPlayer::prepareAsync() {

    return 0;
}

int  YtxMediaPlayer::start() {

    fp_yuv = fopen("/storage/emulated/0/output.yuv","wb+");
    fp_pcm = fopen("/storage/emulated/0/output.pcm","wb+");
    char datam[1024]="ytxh ytxhaooooolkaslfasl;'kf'as ";
   // fwrite(datam,1,1024,fp_yuv);
    ALOGI("start fp_yuv=%d\n",fp_yuv);
    ALOGI("&streamVideo=%d ; streamVideo.dec_ctx=%d\n",&streamVideo,streamVideo.dec_ctx);
    pthread_create(&mPlayerThread, NULL, startPlayer, NULL);
    pthread_create(&mPlayerRefreshThread, NULL, startPlayerRefresh, NULL);
    return 0;
}

void* YtxMediaPlayer::startPlayer(void* ptr)
{
    ALOGI("starting main player thread\n");
  //  printfferr();
    sPlayer->decodeMovie(ptr);
    return 0;
}

void* YtxMediaPlayer::startPlayerRefresh(void* ptr) {
    ALOGI("starting main player startPlayerRefresh\n");
    //  printfferr();
    double last_duration, duration, delay;
    Frame *vp, *lastvp;

    while (sPlayer->isFinish != 1) {
        //usleep(1000);

        if(sPlayer->mDecoderVideo !=NULL){
            usleep(41000);

            if(sPlayer->mDecoderVideo->frameQueue.frameQueueNumRemaining() == 0){
                // nothing to do, no picture to display in the queue

            }else{

                /* dequeue the picture */
           //     lastvp = sPlayer->mDecoderVideo->frameQueue.frameQueuePeekLast();
           //     vp = sPlayer->mDecoderVideo->frameQueue.frameQueuePeek();

                int y_size = sPlayer->streamVideo.dec_ctx->width * sPlayer->streamVideo.dec_ctx->height;
                sPlayer->updateYuv(sPlayer->mYuvFrame->data[0], sPlayer->mYuvFrame->data[1],
                                   sPlayer->mYuvFrame->data[2], y_size);

                sPlayer->mDecoderVideo->frameQueue.frameQueueNext();

            }

        }

    }
    pthread_exit(NULL);
}


void YtxMediaPlayer::decodeMovie(void* ptr)
{
    AVPacket mPacket, *pPacket = &mPacket;
    int i=0, *pI = &i;


    mDecoderAudio = new DecoderAudio(&streamAudio);
    mDecoderAudio->onDecode = decodeAudio;
    mDecoderAudio->startAsync();


    mDecoderVideo = new DecoderVideo(&streamVideo);
    mDecoderVideo->onDecode = decodeVideo;
    mDecoderVideo->onDecodeFinish = finish;
    mDecoderVideo->startAsync();

    mCurrentState = MEDIA_PLAYER_STARTED;
    ALOGI("playing %ix%i\n", mVideoWidth, mVideoHeight);

    while (mCurrentState != MEDIA_PLAYER_DECODED && mCurrentState != MEDIA_PLAYER_STOPPED &&
           mCurrentState != MEDIA_PLAYER_STATE_ERROR )
    {

        if (mDecoderVideo->packets() > MAX_VIDEOQ_SIZE ||
            mDecoderAudio->packets() > MAX_AUDIOQ_SIZE) {
            usleep(100);
            continue;
        }

        int ret = av_read_frame(pFormatCtx, pPacket);
        ALOGI("decodeMovie ret=%d",ret);
        if(ret < 0) {
            mCurrentState = MEDIA_PLAYER_DECODED;
            *pI = 1;
            mDecoderVideo->enqueue(pPacket,pI);
            mDecoderAudio->enqueue(pPacket,pI);
          //  mDecoderVideo->stop();
          //  mDecoderVideo->isFinish = 1;
            continue;
        }
        ALOGI("pPacket->stream_index =%d st_index[AVMEDIA_TYPE_VIDEO]=%d st_index[AVMEDIA_TYPE_AUDIO]=%d\n",pPacket->stream_index ,st_index[AVMEDIA_TYPE_VIDEO],st_index[AVMEDIA_TYPE_AUDIO]);
        if (pPacket->stream_index == st_index[AVMEDIA_TYPE_VIDEO]) {
           // ALOGI("mDecoderVideo->enqueue(pPacket)=%d  *pI=%d",pPacket,*pI);
            mDecoderVideo->enqueue(pPacket,pI);
           // mDecoderVideo->isFinish = 0;
        }else if(pPacket->stream_index == st_index[AVMEDIA_TYPE_AUDIO]){
            mDecoderAudio->enqueue(pPacket,pI);
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
//        timeval pTime;
//        static int frames = 0;
//        static double t1 = -1;
//        static double t2 = -1;
//
//        gettimeofday(&pTime, NULL);
//        t2 = pTime.tv_sec + (pTime.tv_usec / 1000000.0);
//        if (t1 == -1 || t2 > t1 + 1) {
//            ALOGI("Video fps:%i\n", frames);
//            //sPlayer->notify(MEDIA_INFO_FRAMERATE_VIDEO, frames, -1);
//            t1 = t2;
//            frames = 0;
//        }
//        frames++;
//        double time;
//        time = av_gettime_relative() / 1000000.0;




    }

//    usleep(30000);
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
   // ALOGI("y_size=%d ; sPlayer->mYuvFrame->data[0]=%d\n",y_size,sPlayer->mYuvFrame->data[0]);
    //sPlayer->bindTexture(sPlayer->mYuvFrame->data[0],sPlayer->mYuvFrame->data[1],sPlayer->mYuvFrame->data[2]);
   // sPlayer->updateYuv(sPlayer->mYuvFrame->data[0],sPlayer->mYuvFrame->data[1],sPlayer->mYuvFrame->data[2],y_size);

//    double duration;
//
//    AVRational tb = is->video_st->time_base;
//    AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);
//
//    duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);
//    pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
//    ret = queue_picture(is, frame, pts, duration, av_frame_get_pkt_pos(frame), is->viddec.pkt_serial);


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
    sPlayer->isFinish = 1;
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



int YtxMediaPlayer::streamComponentOpen(InputStream *is, int stream_index)
{

    if (stream_index < 0 || stream_index > pFormatCtx->nb_streams) {
        return -1;
    }


    is->dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(is->dec_ctx, pFormatCtx->streams[stream_index]->codecpar);

    av_codec_set_pkt_timebase(is->dec_ctx, pFormatCtx->streams[stream_index]->time_base);

    //  streamVideo.dec_ctx = codec_ctx;
    is->st = pFormatCtx->streams[stream_index];
    //AVCodecContext* codec_ctx = stream->codecpar
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
                out_sample_rate = 44100;
                //获取输入的声道布局
                //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
                //av_get_default_channel_layout(codecCtx->channels);
                out_nb_samples=is->dec_ctx->frame_size;
                ALOGI("out_nb_samples=%d\n",out_nb_samples);
                uint64_t in_ch_layout = is->dec_ctx->channel_layout;
                //输出的声道布局（立体声）
                uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

                ALOGI("### in_ch_layout=%d\n",in_ch_layout);
                ALOGI("### in_sample_rate=%d\n",in_sample_rate);
                ALOGI("### in_sample_fmt=%d\n",in_sample_fmt);

                ALOGI("### out_ch_layout=%d\n",out_ch_layout);
                swr_alloc_set_opts(swrCtx,
                                   out_ch_layout,out_sample_fmt,out_sample_rate,
                                   in_ch_layout,in_sample_fmt,in_sample_rate,
                                   0, NULL);

            /* set options */
//                av_opt_set_int(swrCtx, "in_channel_layout",    in_ch_layout, 0);
//                av_opt_set_int(swrCtx, "in_sample_rate",       in_sample_rate, 0);
//                av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", in_sample_fmt, 0);
//
//
//                av_opt_set_int(swrCtx, "out_channel_layout",    out_ch_layout, 0);
//                av_opt_set_int(swrCtx, "out_sample_rate",       out_sample_rate, 0);
//                av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", out_sample_fmt, 0);

                swr_init(swrCtx);

            //is->dec_ctx->

            //int  dst_nb_samples = av_rescale_rnd(src_nb_samples, out_sample_rate, in_sample_rate, AV_ROUND_UP);


                //输出的声道个数
                out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);

                //重采样设置参数-------------end
                //16bit 44100 PCM 数据
                out_buffer_audio = (uint8_t *)av_malloc(MAX_AUDIO_FRME_SIZE);
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