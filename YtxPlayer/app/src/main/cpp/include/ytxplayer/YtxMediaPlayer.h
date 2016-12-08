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
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <ytxplayer/VideoRefreshController.h>
#include "audio_engine.h"
#include "VideoStateInfo.h"
#include "PlayerPrepareAsync.h"
#include "MessageLoop.h"
#include "MediaPlayerListener.h"
typedef void (*updateYuvHandler) (uint8_t *,uint8_t *,uint8_t *,int);
typedef void (*notifyRendererFrame) ();
// ----------------------------------------------------------------------------



class YtxMediaPlayer {


public:


    YtxMediaPlayer();

    ~YtxMediaPlayer();

    void died();

    void disconnect();

    int setDataSource(const char *url);

    int setDataSource(int fd, int64_t offset, int64_t length);

    //  int        setDataSource(const sp<IStreamSource> &source);
//    int        setVideoSurfaceTexture(
//            const sp<IGraphicBufferProducer>& bufferProducer);
    int setListener(MediaPlayerListener* listener);
    int prepare();

    int prepareAsync();
    PlayerPrepareAsync *mPlayerPrepareAsync;

    int start();

    VideoRefreshController *mVideoRefreshController;
    static void* startPlayer(void* ptr);
    static void* prepareAsyncPlayer(void* ptr);

   // static void* startPlayerRefresh(void* ptr);

    int stop();

    int pause();

    int resume();
    bool isPlaying();

    int getVideoWidth();

    int getVideoHeight();

    int seekTo(int msec);

    int getCurrentPosition(int *msec);

    int getDuration(int *msec);

    int reset();

//    int        setAudioStreamType(audio_stream_type_t type);
    int setLooping(int loop);

    bool isLooping();

    int setVolume(float leftVolume, float rightVolume);

    //   void            notify(int msg, int ext1, int ext2, const Parcel *obj = NULL);
//    static  int        decode(const char* url, uint32_t *pSampleRate, int* pNumChannels,
//                                   audio_format_t* pFormat,
    //                                  const sp<IMemoryHeap>& heap, size_t *pSize);
//    static  int        decode(int fd, int64_t offset, int64_t length, uint32_t *pSampleRate,
//                                   int* pNumChannels, audio_format_t* pFormat,
//                                   const sp<IMemoryHeap>& heap, size_t *pSize);
    //   int        invoke(const Parcel& request, Parcel *reply);
//    int        setMetadataFilter(const Parcel& filter);
//    int        getMetadata(bool update_only, bool apply_filter, Parcel *metadata);
    int setAudioSessionId(int sessionId);

    int getAudioSessionId();

    int setAuxEffectSendLevel(float level);

    int attachAuxEffect(int effectId);

    //   int        setParameter(int key, const Parcel& request);
//    int        getParameter(int key, Parcel* reply);
    int setRetransmitEndpoint(const char *addrString, uint16_t port);
//    int        setNextMediaPlayer(const sp<MediaPlayer>& player);

    int updateProxyConfig(
            const char *host, int32_t port, const char *exclusionList);


    void notifyRenderer();

    static void decodeAudio(AVFrame* frame, double pts);
    static void decodeVideo(AVFrame* frame, double pts);
    static void decodeAudioFirstFrameHandler();

    static void finish();


    updateYuvHandler updateYuv;
    notifyRendererFrame notifyRendererCallback = NULL;
//    InputStream streamVideo;
//    InputStream streamAudio;

    int isFinish;
    DecoderVideo  *mDecoderVideo;
    DecoderAudio  *mDecoderAudio;
    MessageLoop   *mMessageLoop;
    VideoStateInfo *mVideoStateInfo;
//private:

    int streamComponentOpen(InputStream *is, int stream_index);
    void decodeMovie(void* ptr);
    void clear_l();

    int seekTo_l(int msec);

    int prepareAsync_l();

    int getDuration_l(int *msec);

    //  int        attachNewPlayer(const sp<IMediaPlayer>& player);
    int reset_l();
    //  int        doSetRetransmitEndpoint(const sp<IMediaPlayer>& player);
    YtxMediaPlayer *mPlayer;
    // thread_id_t                 mLockThreadId;
    //  Mutex                       mLock;
    //  Mutex                       mNotifyLock;
    //  Condition                   mSignal;
    MediaPlayerListener     *mListener;
    void *mCookie;
    //  media_player_states         mCurrentState;
    int mCurrentPosition;
    int mSeekPosition;
    bool mPrepareSync;
    int mPrepareStatus;
    //  audio_stream_type_t         mStreamType;
    bool mLoop;
    float mLeftVolume;
    float mRightVolume;
    int mVideoWidth;
    int mVideoHeight;
    int mAudioSessionId;
    float mSendLevel;
    //  struct sockaddr_in          mRetransmitEndpoint;
    bool mRetransmitEndpointValid;
    const char *filePath;
    AVFormatContext *pFormatCtx;
    int mAudioStreamIndex;
    int mVideoStreamIndex;
    const char* wanted_stream_spec[AVMEDIA_TYPE_NB]={0};
    int  mDuration;
    struct SwsContext*	mConvertCtx;
    SwrContext *swrCtx;
    int abortRequest;



    AVFrame*					mFrameVideo;
    AVFrame*					mFrameAudio;
    AVFrame*					mYuvFrame;
    pthread_mutex_t             mLock;
    pthread_t					mPlayerThread;
    pthread_t					mPlayerPrepareAsyncThread;

    int mStreamType;
    int mCurrentState;
    int st_index[AVMEDIA_TYPE_NB];


    FILE *fp_yuv;
    FILE *fp_pcm;
    int  got_picture;
    unsigned char *out_buffer_video;
    unsigned char *out_buffer_audio;

    //视频帧率
    AVRational frame_rate_video;


    enum AVSampleFormat in_sample_fmt ;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    int in_sample_rate ;
    //输出采样率
    int out_sample_rate = 44100;
    int out_channel_nb;
    int out_nb_samples;

    AudioEngine *audioEngine;
    bool playing = false;
};


#endif //YTXPLAYER_YTXMEDIAPLAYER_H
