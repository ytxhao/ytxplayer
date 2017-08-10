//
// Created by Administrator on 2016/9/2.
//

#define LOG_NDEBUG 0
#define TAG "YTX-PLAYER-JNI"


#include <ytxplayer/utils.h>
#include <ytxplayer/GLThread.h>
#include "ytxplayer/ALog-priv.h"

//该文件必须包含在源文件中(*.cpp),以免宏展开时提示重复定义的错误
#include "ytxplayer/YtxMediaPlayer.h"
#include "ytxplayer/ffmsg.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "SdlAoutOpensles.h"

#ifdef __cplusplus
}
#endif

void printferr() {
    ALOGI("AVERROR_BSF_NOT_FOUND=%d\n", AVERROR_BSF_NOT_FOUND);
    ALOGI("AVERROR_BUG=%d\n", AVERROR_BUG);
    ALOGI("AVERROR_BUFFER_TOO_SMALL=%d\n", AVERROR_BUFFER_TOO_SMALL);
    ALOGI("AVERROR_DECODER_NOT_FOUND=%d\n", AVERROR_DECODER_NOT_FOUND);
    ALOGI("AVERROR_DEMUXER_NOT_FOUND=%d\n", AVERROR_DEMUXER_NOT_FOUND);
    ALOGI("AVERROR_ENCODER_NOT_FOUND=%d\n", AVERROR_ENCODER_NOT_FOUND);
    ALOGI("AVERROR_EOF=%d\n", AVERROR_EOF);
    ALOGI("AVERROR_EXIT=%d\n", AVERROR_EXIT);
    ALOGI("AVERROR_EXTERNAL=%d\n", AVERROR_EXTERNAL);
    ALOGI("AVERROR_FILTER_NOT_FOUND=%d\n", AVERROR_FILTER_NOT_FOUND);
    ALOGI("AVERROR_INVALIDDATA=%d\n", AVERROR_INVALIDDATA);
    ALOGI("AVERROR_MUXER_NOT_FOUND=%d\n", AVERROR_MUXER_NOT_FOUND);
    ALOGI("AVERROR_OPTION_NOT_FOUND=%d\n", AVERROR_OPTION_NOT_FOUND);
    ALOGI("AVERROR_PATCHWELCOME=%d\n", AVERROR_PATCHWELCOME);
    ALOGI("AVERROR_PROTOCOL_NOT_FOUND=%d\n", AVERROR_PROTOCOL_NOT_FOUND);
    ALOGI("AVERROR_STREAM_NOT_FOUND=%d\n", AVERROR_STREAM_NOT_FOUND);
/**
 * This is semantically identical to AVERROR_BUG
 * it has been introduced in Libav after our AVERROR_BUG and with a modified value.
 */
    ALOGI("AVERROR_BUG2=%d\n", AVERROR_BUG2);
    ALOGI("AVERROR_UNKNOWN=%d\n", AVERROR_UNKNOWN);
    ALOGI("AVERROR_EXPERIMENTAL=%d\n", AVERROR_EXPERIMENTAL);
    ALOGI("AVERROR_INPUT_CHANGED=%d\n", AVERROR_INPUT_CHANGED);
    ALOGI("AVERROR_OUTPUT_CHANGED=%d\n", AVERROR_OUTPUT_CHANGED);
/* HTTP & RTSP errors */
    ALOGI("AVERROR_HTTP_BAD_REQUEST=%d\n", AVERROR_HTTP_BAD_REQUEST);
    ALOGI("AVERROR_HTTP_UNAUTHORIZED=%d\n", AVERROR_HTTP_UNAUTHORIZED);
    ALOGI("AVERROR_HTTP_FORBIDDEN=%d\n", AVERROR_HTTP_FORBIDDEN);
    ALOGI("AVERROR_HTTP_NOT_FOUND=%d\n", AVERROR_HTTP_NOT_FOUND);
    ALOGI("AVERROR_HTTP_OTHER_4XX=%d\n", AVERROR_HTTP_OTHER_4XX);
    ALOGI("AVERROR_HTTP_SERVER_ERROR=%d\n", AVERROR_HTTP_SERVER_ERROR);
}

YtxMediaPlayer::YtxMediaPlayer() {
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
    mVideoStateInfo = new VideoStateInfo();
    mVideoStateInfo->mCurrentState = &mCurrentState;
    mVideoRefreshController = NULL;
    //mAudioRefreshController = NULL;
    //mGLThread = NULL;
    isRelease = false;
    mDecoderSubtitle = NULL;
    mDecoderVideo = NULL;
    mDecoderAudio = NULL;
    subtitles = NULL;
    memset(fileType,0,sizeof(fileType));
}

YtxMediaPlayer::~YtxMediaPlayer() {

    //  AudioEngine::releaseAudioEngine();
    //  GlEngine::releaseGlEngine();

    if (mDecoderSubtitle) {
        delete mDecoderSubtitle;
    }

    if (mDecoderVideo) {
        delete mDecoderVideo;
    }

    if (mDecoderAudio) {
        delete mDecoderAudio;
    }

    if (mVideoRefreshController) {
        delete mVideoRefreshController;
    }

//    if (mAudioRefreshController) {
//        delete mAudioRefreshController;
//    }

//    if(mGLThread){
//        delete mGLThread;
//    }

    delete mVideoStateInfo;


    if (mListener != NULL) {
        mListener->notify(MEDIA_STOPPED, 0, 0);
    }

}

void YtxMediaPlayer::died() {

}


void YtxMediaPlayer::disconnect() {

}

int YtxMediaPlayer::setDataSource(const char *url) {

    this->filePath = url;
    ALOGI("YtxMediaPlayer setDataSource filePath=%s\n", filePath);
    mCurrentState = MEDIA_PLAYER_INITIALIZED;
    return 0;
}

int YtxMediaPlayer::setSubtitles(char *url) {

    this->subtitles = url;
    ALOGI("YtxMediaPlayer setSubtitles subtitles=%s\n", subtitles);
    return 0;
}


int YtxMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {

    return 0;
}


int YtxMediaPlayer::prepare() {

    return 0;

}

void YtxMediaPlayer::getFileType(const char* fileName, char* outputFileType)
{
    int i,j;
    char ch;
    i = strlen(fileName) - 1;
    for(outputFileType[0]='\0';i>=0;i--)
    {
        if(fileName[i] == '.')
        {
            // 遇到文件类型分隔符
            for(j=i; fileName[j]!='\0'; j++)
            {
                ch = fileName[j];
                outputFileType[j-i] = ('A'<=ch && ch<='Z') ? (ch+'a'-'A'): ch;
            }
            outputFileType[j-i] = '\0';
            break;
        }
    }

}

int YtxMediaPlayer::prepareAsync() {

    mCurrentState = MEDIA_PLAYER_PREPARING;
    getFileType(filePath,fileType);
    ALOGI("prepareAsync prepare fileType = %s",fileType);
    pthread_create(&mPlayerPrepareAsyncThread, NULL, prepareAsyncPlayer, this);
    return 0;
}

void *YtxMediaPlayer::prepareAsyncPlayer(void *ptr) {


    YtxMediaPlayer *mPlayer = (YtxMediaPlayer *) ptr;
    avfilter_register_all(); //添加滤镜
    av_register_all();
    avformat_network_init();
    mPlayer->mVideoStateInfo->pFormatCtx = avformat_alloc_context();

    ALOGI("prepareAsyncPlayer prepare this->filePath=%s\n", mPlayer->filePath);
    //   ALOGI("Couldn't open input stream.\n");
    if (avformat_open_input(&mPlayer->mVideoStateInfo->pFormatCtx, mPlayer->filePath, NULL, NULL) !=
        0) {
        ALOGE("Couldn't open input stream.\n");
        AVMessage msg;
        msg.what = FFP_MSG_ERROR;
        msg.arg1 = MEDIA_ERROR_OPEN_STREAM;
        mPlayer->mVideoStateInfo->mMessageLoop->enqueue(&msg);
        mPlayer->mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        avformat_network_deinit();
        pthread_exit(NULL);
    }

    if (avformat_find_stream_info(mPlayer->mVideoStateInfo->pFormatCtx, NULL) < 0) {
        ALOGE("Couldn't find stream information.\n");
        return 0;
    }


    for (int i = 0; i < mPlayer->mVideoStateInfo->pFormatCtx->nb_streams; i++) {
        AVStream *st = mPlayer->mVideoStateInfo->pFormatCtx->streams[i];
        enum AVMediaType type = st->codecpar->codec_type;

        if (type >= 0 && mPlayer->wanted_stream_spec[type] &&
            mPlayer->mVideoStateInfo->st_index[type] == -1) {
            if (avformat_match_stream_specifier(mPlayer->mVideoStateInfo->pFormatCtx, st,
                                                mPlayer->wanted_stream_spec[type]) > 0) {
                mPlayer->mVideoStateInfo->st_index[type] = i;
            }
        }

    }

    for (int i = 0; i < AVMEDIA_TYPE_NB; i++) {
        if (mPlayer->wanted_stream_spec[i] && mPlayer->mVideoStateInfo->st_index[i] == -1) {
            ALOGI("Stream specifier %s does not match any %s stream\n",
                  mPlayer->wanted_stream_spec[(AVMediaType) i],
                  av_get_media_type_string((AVMediaType) i));
            mPlayer->mVideoStateInfo->st_index[i] = -1;
        }
    }


    //mPlayer->mVideoStateInfo->pFormatCtx = mPlayer->pFormatCtx;
    mPlayer->mVideoStateInfo->max_frame_duration = (mPlayer->mVideoStateInfo->pFormatCtx->iformat->flags &
                                                    AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

    ALOGI("mVideoStateInfo->max_frame_duration=%lf\n",
          mPlayer->mVideoStateInfo->max_frame_duration);
    if (mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        mPlayer->streamComponentOpen(mPlayer->mVideoStateInfo->streamAudio,
                                     mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO]);
        mPlayer->mDecoderAudio = new DecoderAudio(mPlayer->mVideoStateInfo);
    }

    if (mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        mPlayer->streamComponentOpen(mPlayer->mVideoStateInfo->streamVideo,
                                     mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO]);
        mPlayer->mDecoderVideo = new DecoderVideo(mPlayer->mVideoStateInfo);
    }

    if (mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
        mPlayer->streamComponentOpen(mPlayer->mVideoStateInfo->streamSubtitle,
                                     mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE]);
        mPlayer->mDecoderSubtitle = new DecoderSubtitle(mPlayer->mVideoStateInfo);
    }

    if (mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0 ||
        mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0 ||
        mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        mPlayer->mVideoStateInfo->initClock(mPlayer->mVideoStateInfo->extClk,
                                            &mPlayer->mVideoStateInfo->extClk->serial);
        mPlayer->mVideoStateInfo->setClockSpeed(mPlayer->mVideoStateInfo->extClk, 1);
    }


    if (mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0 &&
        mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] < 0 &&
        mPlayer->mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] < 0) {
        AVMessage msg;
        msg.what = FFP_MSG_ERROR;
        msg.arg1 = MEDIA_ERROR_OPEN_STREAM_IS_SUBTITLES;
        mPlayer->mVideoStateInfo->mMessageLoop->enqueue(&msg);
        mPlayer->mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        avformat_network_deinit();
        pthread_exit(NULL);
    }

    mPlayer->mVideoStateInfo->frameQueueSubtitle->frameQueueInit(SUBPICTURE_QUEUE_SIZE, 0);
    mPlayer->mVideoStateInfo->frameQueueVideo->frameQueueInit(VIDEO_PICTURE_QUEUE_SIZE, 1);
    mPlayer->mVideoStateInfo->frameQueueAudio->frameQueueInit(SAMPLE_QUEUE_SIZE, 1);


    mPlayer->mVideoRefreshController = new VideoRefreshController(mPlayer->mVideoStateInfo);

    //mPlayer->mAudioRefreshController = new AudioRefreshController(mPlayer->mVideoStateInfo);

    mPlayer->mVideoStateInfo->vfilters_list = (const char **) GROW_ARRAY(
            mPlayer->mVideoStateInfo->vfilters_list, mPlayer->mVideoStateInfo->nb_vfilters);


    ALOGI("mPlayer->subtitles=%s", mPlayer->subtitles);
    if (mPlayer->subtitles != NULL) {
        mPlayer->mVideoStateInfo->vfilters_list[mPlayer->mVideoStateInfo->nb_vfilters -
                                                1] = mPlayer->mVideoStateInfo->join(
                "subtitles=", mPlayer->subtitles);
    }

    ALOGI("vfilters_list = %s",
          mPlayer->mVideoStateInfo->vfilters_list[mPlayer->mVideoStateInfo->nb_vfilters - 1]);

    mPlayer->mVideoStateInfo->init_opts();

    AVMessage msgVideoSize;
    msgVideoSize.what = FFP_MSG_VIDEO_SIZE_CHANGED;
    msgVideoSize.arg1 = mPlayer->mVideoStateInfo->mVideoWidth;
    msgVideoSize.arg2 = mPlayer->mVideoStateInfo->mVideoHeight;
    mPlayer->mVideoStateInfo->mMessageLoop->enqueue(&msgVideoSize);

    mPlayer->mVideoStateInfo->aout = SDL_AoutAndroid_CreateForOpenSLES();
    AVMessage msg;
    msg.what = FFP_MSG_PREPARED;
    if(mPlayer->mVideoStateInfo->aout == NULL){
        msg.what = FFP_MSG_ERROR;
    }

    mPlayer->mVideoStateInfo->mMessageLoop->enqueue(&msg);
    mPlayer->mCurrentState = MEDIA_PLAYER_PREPARED;
    pthread_exit(NULL);
}


int YtxMediaPlayer::resume() {
    mCurrentState = MEDIA_PLAYER_STARTED;
    mVideoStateInfo->notifyAll();
    return 0;
}

int YtxMediaPlayer::start() {
    ALOGI("YtxMediaPlayer::start() mCurrentState=%d \n", mCurrentState);

    if (mCurrentState == MEDIA_PLAYER_PAUSED) {
        return resume();

    }

    pthread_create(&mPlayerThread, NULL, startPlayer, this);

    return 0;
}

void *YtxMediaPlayer::startPlayer(void *ptr) {
    ALOGI("starting main player thread\n");
    YtxMediaPlayer *mPlayer = (YtxMediaPlayer *) ptr;
    printferr();
    //等待surface render初始化完成

//    mPlayer->mGLThread->startAsync();
//    do {
//        usleep(50);
//    } while (rendererStarted(mPlayer->mVideoStateInfo->GraphicRendererObj) != 1);

    mPlayer->decodeMovie(ptr);

    if (mPlayer->mVideoStateInfo->pFormatCtx != NULL) {
        avformat_close_input(&mPlayer->mVideoStateInfo->pFormatCtx);
    }
    mPlayer->mVideoStateInfo->uninit_opts();
    av_freep(&mPlayer->mVideoStateInfo->vfilters_list);
    avformat_network_deinit();
    if (mPlayer != NULL) {
        delete mPlayer;
    }

    return 0;
}

void YtxMediaPlayer::checkSeekRequest() {

    int ret;
    if (mVideoStateInfo != NULL) {
        if (mVideoStateInfo->seekReq) {
            int64_t seek_target = mVideoStateInfo->seekPos;
            int64_t seek_min =
                    mVideoStateInfo->seekRel > 0 ? seek_target - mVideoStateInfo->seekRel + 2
                                                 : INT64_MIN;
            int64_t seek_max =
                    mVideoStateInfo->seekRel < 0 ? seek_target - mVideoStateInfo->seekRel - 2
                                                 : INT64_MAX;
            ALOGI("checkSeekRequest seek_target=%lld seek_min=%lld seek_max=%lld mVideoStateInfo->seekFlags=%d\n",
                  seek_target, seek_min, seek_max, mVideoStateInfo->seekFlags);
            ret = avformat_seek_file(mVideoStateInfo->pFormatCtx, -1, seek_min, seek_target,
                                     seek_max, mVideoStateInfo->seekFlags);
            ALOGI("avformat_seek_file ret = %d\n", ret);
            if (ret < 0) {
                ALOGE("%s: error while seeking\n", filePath);
            } else {
                if (mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
                    mDecoderAudio->flush();
                    mDecoderAudio->enqueue(mVideoStateInfo->flushPkt);
                }

                if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
                    mDecoderVideo->flush();
                    mDecoderVideo->enqueue(mVideoStateInfo->flushPkt);
                }
                if (mVideoStateInfo->seekFlags & AVSEEK_FLAG_BYTE) {
                    // set_clock(&is->extclk, NAN, 0);
                } else {
                    // set_clock(&is->extclk, seek_target / (double)AV_TIME_BASE, 0);
                    mVideoStateInfo->setClock(mVideoStateInfo->extClk,
                                              seek_target / (double) AV_TIME_BASE, 0);
                }
            }

            mVideoStateInfo->seekReq = false;
            mVideoStateInfo->eof = 0;
        }
    }


}


void YtxMediaPlayer::decodeMovie(void *ptr) {

    MAVPacket mPacket, *pPacket = &mPacket;


    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        mVideoRefreshController->startAsync();
        mDecoderVideo->startAsync();
        //mGLThread->startAsync();
    }

    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        //mAudioRefreshController->startAsync();
        mDecoderAudio->startAsync();
    }

    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
        mDecoderSubtitle->startAsync();
    }

    mCurrentState = MEDIA_PLAYER_STARTED;

    while (mCurrentState != MEDIA_PLAYER_DECODED && mCurrentState != MEDIA_PLAYER_STOPPED &&
           mCurrentState != MEDIA_PLAYER_STATE_ERROR) {

        if (mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0 &&
            mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0
            && mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {

            if (mDecoderVideo->packets() + mDecoderAudio->packets() + mDecoderSubtitle->packets() >
                MAX_QUEUE_SIZE ||
                mDecoderVideo->streamHasEnoughPackets() &&
                mDecoderAudio->streamHasEnoughPackets() &&
                mDecoderSubtitle->streamHasEnoughPackets()) {
                packetEnoughWait();
                continue;
            }
        } else if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0
                   && mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {

            if (mDecoderVideo->packets() + mDecoderAudio->packets() > MAX_QUEUE_SIZE ||
                mDecoderVideo->streamHasEnoughPackets() &&
                mDecoderAudio->streamHasEnoughPackets()) {
                packetEnoughWait();
                continue;
            }
        } else if (mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
            if (mDecoderAudio->packets() > MAX_QUEUE_SIZE ||
                mDecoderAudio->streamHasEnoughPackets()) {
                packetEnoughWait();
                continue;
            }
        }


        checkSeekRequest();

        int ret = av_read_frame(mVideoStateInfo->pFormatCtx, &pPacket->pkt);

        if (ret < 0) {

            if (ret == AVERROR_EOF ||
                avio_feof(mVideoStateInfo->pFormatCtx->pb) && !mVideoStateInfo->eof) {

                if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
                    mDecoderVideo->enqueueNullPacket(mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO]);
                }
                if (mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
                    mDecoderAudio->enqueueNullPacket(mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO]);
                }
                mVideoStateInfo->eof = 1;

            }

            if (mVideoStateInfo->pFormatCtx->pb && mVideoStateInfo->pFormatCtx->pb->error) {
                break;
            }

            pthread_mutex_lock(&mVideoStateInfo->wait_mutex);
            pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
            pthread_mutex_unlock(&mVideoStateInfo->wait_mutex);

            continue;
        } else {
            mVideoStateInfo->eof = 0;
        }

        if (pPacket->pkt.stream_index == mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO]) {
            mDecoderVideo->enqueue(pPacket);
        } else if (pPacket->pkt.stream_index == mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO]) {
            mDecoderAudio->enqueue(pPacket);
        } else if (pPacket->pkt.stream_index == mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE]) {
            mDecoderSubtitle->enqueue(pPacket);
        } else {
            av_packet_unref(&pPacket->pkt);
        }
    }

    mVideoStateInfo->notifyAll();


    int ret = -1;
    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
        mDecoderSubtitle->stop();
        ALOGI("waiting on mDecoderSubtitle thread\n");
        if ((ret = mDecoderSubtitle->wait()) != 0) {
            ALOGE("Couldn't cancel subtitle thread: %i", ret);
        }
    }

    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
        mVideoRefreshController->stop();
        mDecoderVideo->stop();
        ALOGI("waiting on mDecoderVideo thread\n");
        if ((ret = mDecoderVideo->wait()) != 0) {
            ALOGE("Couldn't cancel video thread: %i", ret);
        }
    }

    if (mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        //mAudioRefreshController->stop();
        mDecoderAudio->stop();
        ALOGI("waiting on mDecoderAudio thread\n");
        if ((ret = mDecoderAudio->wait()) != 0) {
            ALOGE("Couldn't cancel audio thread: %i", ret);
        }
    }

    finish();

    ALOGI("end of playing\n");
}


void YtxMediaPlayer::packetEnoughWait() {
    /* wait 10 ms */
    struct timeval now;
    struct timespec outTime;
    int timeout_ms = 10;
    int nsec = 0;
    pthread_mutex_lock(&mVideoStateInfo->wait_mutex);
    gettimeofday(&now, NULL);
    nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
    outTime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000; //now.tv_sec + 5;
    outTime.tv_nsec = nsec % 1000000000; //now.tv_usec * 1000;
    pthread_cond_timedwait(&mVideoStateInfo->continue_read_thread, &mVideoStateInfo->wait_mutex,
                           &outTime);
    pthread_mutex_unlock(&mVideoStateInfo->wait_mutex);


}

int YtxMediaPlayer::release() {
    ALOGI("YtxMediaPlayer::release");
    isRelease = true;
    return 0;
}

int YtxMediaPlayer::stop() {
    ALOGI("YtxMediaPlayer::stop");

    mCurrentState = MEDIA_PLAYER_STOPPED;
    mVideoStateInfo->mMessageLoop->stop();
    mVideoStateInfo->abort_request = 1;
    pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    // playing = false;
    return 0;
}

int YtxMediaPlayer::pause() {
    ALOGI("YtxMediaPlayer::pause");
    mCurrentState = MEDIA_PLAYER_PAUSED;
    return 0;
}

bool YtxMediaPlayer::isPlaying() {

    return mCurrentState == MEDIA_PLAYER_STARTED || mCurrentState == MEDIA_PLAYER_DECODED;
}

int YtxMediaPlayer::getVideoWidth() {

    return mVideoStateInfo->mVideoWidth;
}

int YtxMediaPlayer::getVideoHeight() {

    return mVideoStateInfo->mVideoHeight;
}

int YtxMediaPlayer::seekTo(int msec) {

    mVideoStateInfo->seekPos = (int64_t) (((double) msec / getDuration()) *
                                          mVideoStateInfo->pFormatCtx->duration);
    mVideoStateInfo->seekRel = 0;
    mVideoStateInfo->seekReq = true;
    pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    ALOGI("seekTo seekPos=%lld seekRel=%lld msec=%d\n", mVideoStateInfo->seekPos,
          mVideoStateInfo->seekRel, msec);

    return 0;
}

int YtxMediaPlayer::getCurrentPosition() {
    return mVideoStateInfo->currentTime;
}

int YtxMediaPlayer::getDuration() {
    int64_t ts;
    int ns, hh, mm, ss;
    int tns, thh, tmm, tss, tms;
    ts = mVideoStateInfo->pFormatCtx->duration;
    tns = ts / 1000000LL; //总共有多少秒
    thh = tns / 3600;
    tmm = (tns % 3600) / 60;
    tss = (tns % 60);

    tms = ts / 1000LL; //总共有多少毫秒
//    frac = x / cur_stream->width;
//    ns   = frac * tns;
//    hh   = ns / 3600;
//    mm   = (ns % 3600) / 60;
//    ss   = (ns % 60);

    return tms;
}

int YtxMediaPlayer::reset() {

    return 0;
}


int YtxMediaPlayer::setLooping(int loop) {

    return 0;
}

bool YtxMediaPlayer::isLooping() {

    return 0;
}

int YtxMediaPlayer::setVolume(float leftVolume, float rightVolume) {

    return 0;
}

int YtxMediaPlayer::setAudioSessionId(int sessionId) {

    return 0;
}

int YtxMediaPlayer::getAudioSessionId() {

    return 0;
}

int YtxMediaPlayer::setAuxEffectSendLevel(float level) {

    return 0;
}

int YtxMediaPlayer::attachAuxEffect(int effectId) {

    return 0;
}

int YtxMediaPlayer::setRetransmitEndpoint(const char *addrString, uint16_t port) {

    return 0;
}


int YtxMediaPlayer::updateProxyConfig(const char *host, int32_t port, const char *exclusionList) {

    return 0;
}

void YtxMediaPlayer::clear_l() {


}

int YtxMediaPlayer::seekTo_l(int msec) {

    return 0;
}

int YtxMediaPlayer::prepareAsync_l() {

    return 0;
}

int YtxMediaPlayer::getDuration_l(int *msec) {

    return 0;
}

int YtxMediaPlayer::reset_l() {

    return 0;
}

int YtxMediaPlayer::setListener(MediaPlayerListener *listener) {

    mListener = listener;
    mVideoStateInfo->mMessageLoop->setMsgListener(mListener);
    mVideoStateInfo->mMessageLoop->startAsync();
    return 0;
}

void YtxMediaPlayer::finish() {

    ALOGI("YtxMediaPlayer::finish IN");
    // mVideoStateInfo->mMessageLoop->stop();
    isFinish = 1;
    ALOGI("YtxMediaPlayer::finish OUT");
}


int YtxMediaPlayer::streamComponentOpen(InputStream *is, int stream_index) {

    if (stream_index < 0 || stream_index > mVideoStateInfo->pFormatCtx->nb_streams) {
        return -1;
    }

    is->dec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(is->dec_ctx,
                                  mVideoStateInfo->pFormatCtx->streams[stream_index]->codecpar);

    av_codec_set_pkt_timebase(is->dec_ctx,
                              mVideoStateInfo->pFormatCtx->streams[stream_index]->time_base);
    is->st = mVideoStateInfo->pFormatCtx->streams[stream_index];
    AVCodec *codec = avcodec_find_decoder(is->dec_ctx->codec_id);
    if (codec == NULL) {
        return -1;
    }

    ALOGI("streamVideo.dec_ctx->codec_id=%d\n", is->dec_ctx->codec_id);

    is->dec_ctx->codec_id = codec->id;

    // Open codec
    if (avcodec_open2(is->dec_ctx, codec, NULL) < 0) {
        return -1;
    }


    switch (is->dec_ctx->codec_type) {
        case AVMEDIA_TYPE_AUDIO: {
            //解压缩数据
            mFrameAudio = av_frame_alloc();
            //frame->16bit 44100 PCM 统一音频采样格式与采样率
            //重采样设置参数-------------start
            //输入的采样格式
            mVideoStateInfo->in_sample_fmt = is->dec_ctx->sample_fmt;
            //输出采样格式16bit PCM
            mVideoStateInfo->out_sample_fmt = AV_SAMPLE_FMT_S16;
            //输入采样率
            mVideoStateInfo->in_sample_rate = is->dec_ctx->sample_rate;
            //输出采样率
            //out_sample_rate = 44100;
            mVideoStateInfo->out_sample_rate = mVideoStateInfo->in_sample_rate;
            //获取输入的声道布局
            //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
            //av_get_default_channel_layout(codecCtx->channels);
            mVideoStateInfo->out_nb_samples = is->dec_ctx->frame_size;

            mVideoStateInfo->in_ch_layout = is->dec_ctx->channel_layout;
            //输出的声道布局（立体声）
            mVideoStateInfo->out_ch_layout = AV_CH_LAYOUT_STEREO;

            //输出的声道个数
            mVideoStateInfo->out_channel_nb = av_get_channel_layout_nb_channels(
                    mVideoStateInfo->out_ch_layout);

            //重采样设置参数-------------end

            ALOGI("### in_sample_rate=%d\n", mVideoStateInfo->in_sample_rate);
            ALOGI("### in_sample_fmt=%d\n", mVideoStateInfo->in_sample_fmt);
            ALOGI("### out_nb_samples=%d\n", mVideoStateInfo->out_nb_samples);
            ALOGI("### out_sample_rate=%d\n", mVideoStateInfo->out_sample_rate);
            ALOGI("### out_sample_fmt=%d\n", mVideoStateInfo->out_sample_fmt);
            ALOGI("### out_channel_nb=%d\n", mVideoStateInfo->out_channel_nb);

            ALOGI("### in_ch_layout=%d\n", mVideoStateInfo->in_ch_layout);
            ALOGI("### out_ch_layout=%d\n", mVideoStateInfo->out_ch_layout);
        }
            break;
        case AVMEDIA_TYPE_VIDEO:

            mVideoStateInfo->mVideoWidth = is->dec_ctx->width;
            mVideoStateInfo->mVideoHeight = is->dec_ctx->height;
            mDuration = mVideoStateInfo->pFormatCtx->duration;

            mFrameVideo = av_frame_alloc();
            mYuvFrame = av_frame_alloc();

            if (mFrameVideo == NULL || mYuvFrame == NULL) {
                return -1;
            }

            break;
        case AVMEDIA_TYPE_SUBTITLE:

            ALOGI("AVMEDIA_TYPE_SUBTITLE");

            break;
    }


    return 0;
}