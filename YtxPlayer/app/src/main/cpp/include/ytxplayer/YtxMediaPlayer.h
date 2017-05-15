//
// Created by Administrator on 2016/9/2.
//

#ifndef YTXPLAYER_YTXMEDIAPLAYER_H
#define YTXPLAYER_YTXMEDIAPLAYER_H


#include <stdint.h>
#include <pthread.h>
#include "merrors.h"
#include "ffinc.h"
#include "decoder_video.h"
#include "decoder_audio.h"
#include "decoder_subtitle.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <ytxplayer/VideoRefreshController.h>
#include <ytxplayer/AudioRefreshController.h>
#include <ytxplayer/GLThread.h>
#include "audio_engine.h"
#include "VideoStateInfo.h"
#include "PlayerPrepareAsync.h"
#include "MessageLoop.h"
#include "MediaPlayerListener.h"


// ----------------------------------------------------------------------------



class YtxMediaPlayer {


public:


    YtxMediaPlayer();

    ~YtxMediaPlayer();

    void died();

    void disconnect();

    int setSubtitles( char *url);
    int setDataSource(const char *url);

    int setDataSource(int fd, int64_t offset, int64_t length);

    int setListener(MediaPlayerListener* listener);
    int prepare();

    int prepareAsync();
    PlayerPrepareAsync *mPlayerPrepareAsync;

    int start();

    VideoRefreshController *mVideoRefreshController;
    AudioRefreshController *mAudioRefreshController;
    //GLThread *mGLThread;
    static void* startPlayer(void* ptr);
    static void* prepareAsyncPlayer(void* ptr);


    void packetEnoughWait();
    int stop();

    bool isRelease;
    int release();

    int pause();

    int resume();
    bool isPlaying();

    int getVideoWidth();

    int getVideoHeight();

    void checkSeekRequest();
    int seekTo(int msec);

    int getCurrentPosition();

    int getDuration();

    int reset();


    int setLooping(int loop);

    bool isLooping();

    int setVolume(float leftVolume, float rightVolume);


    int setAudioSessionId(int sessionId);

    int getAudioSessionId();

    int setAuxEffectSendLevel(float level);

    int attachAuxEffect(int effectId);


    int setRetransmitEndpoint(const char *addrString, uint16_t port);

    int updateProxyConfig(
            const char *host, int32_t port, const char *exclusionList);


    void notifyRenderer();

    void finish();

    int isFinish;
    DecoderVideo  *mDecoderVideo;
    DecoderAudio  *mDecoderAudio;
    DecoderSubtitle  *mDecoderSubtitle;

    VideoStateInfo *mVideoStateInfo;
//private:

    int streamComponentOpen(InputStream *is, int stream_index);
    void decodeMovie(void* ptr);
    void clear_l();

    int seekTo_l(int msec);

    int prepareAsync_l();

    int getDuration_l(int *msec);

    int reset_l();

    YtxMediaPlayer *mPlayer;

    MediaPlayerListener     *mListener;
    void *mCookie;

    int mCurrentPosition;
    int mSeekPosition;
    bool mPrepareSync;
    int mPrepareStatus;

    bool mLoop;
    float mLeftVolume;
    float mRightVolume;

    int mAudioSessionId;
    float mSendLevel;

    bool mRetransmitEndpointValid;
    const char *filePath;
    char *subtitles=NULL;
    //AVFormatContext *pFormatCtx;
    int mAudioStreamIndex;
    int mVideoStreamIndex;
    const char* wanted_stream_spec[AVMEDIA_TYPE_NB]={0};
    int  mDuration;

    int abortRequest;

    AVFrame*					mFrameVideo;
    AVFrame*					mFrameAudio;
    AVFrame*					mYuvFrame;
    pthread_mutex_t             mLock;
    pthread_t					mPlayerThread;
    pthread_t					mPlayerPrepareAsyncThread;

    int mStreamType;
    int mCurrentState;

    int  got_picture;

};


#endif //YTXPLAYER_YTXMEDIAPLAYER_H
