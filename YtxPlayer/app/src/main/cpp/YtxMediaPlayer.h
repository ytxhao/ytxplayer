//
// Created by Administrator on 2016/9/2.
//

#ifndef YTXPLAYER_YTXMEDIAPLAYER_H
#define YTXPLAYER_YTXMEDIAPLAYER_H


#include <stdint.h>

class YtxMediaPlayer{


public:
    YtxMediaPlayer();
    ~YtxMediaPlayer();

    void            died();
    void            disconnect();

    int        setDataSource(
            const char *url);

    int        setDataSource(int fd, int64_t offset, int64_t length);
    int        setDataSource(const sp<IStreamSource> &source);
    int        setVideoSurfaceTexture(
            const sp<IGraphicBufferProducer>& bufferProducer);
    int        setListener(const sp<MediaPlayerListener>& listener);
    int        prepare();
    int        prepareAsync();
    int        start();
    int        stop();
    int        pause();
    bool            isPlaying();
    int        getVideoWidth(int *w);
    int        getVideoHeight(int *h);
    int        seekTo(int msec);
    int        getCurrentPosition(int *msec);
    int        getDuration(int *msec);
    int        reset();
    int        setAudioStreamType(audio_stream_type_t type);
    int        setLooping(int loop);
    bool            isLooping();
    int        setVolume(float leftVolume, float rightVolume);
    void            notify(int msg, int ext1, int ext2, const Parcel *obj = NULL);
    static  int        decode(const char* url, uint32_t *pSampleRate, int* pNumChannels,
                                   audio_format_t* pFormat,
                                   const sp<IMemoryHeap>& heap, size_t *pSize);
    static  int        decode(int fd, int64_t offset, int64_t length, uint32_t *pSampleRate,
                                   int* pNumChannels, audio_format_t* pFormat,
                                   const sp<IMemoryHeap>& heap, size_t *pSize);
    int        invoke(const Parcel& request, Parcel *reply);
    int        setMetadataFilter(const Parcel& filter);
    int        getMetadata(bool update_only, bool apply_filter, Parcel *metadata);
    int        setAudioSessionId(int sessionId);
    int             getAudioSessionId();
    int        setAuxEffectSendLevel(float level);
    int        attachAuxEffect(int effectId);
    int        setParameter(int key, const Parcel& request);
    int        getParameter(int key, Parcel* reply);
    int        setRetransmitEndpoint(const char* addrString, uint16_t port);
    int        setNextMediaPlayer(const sp<MediaPlayer>& player);

    int updateProxyConfig(
            const char *host, int32_t port, const char *exclusionList);

private:
    void            clear_l();
    int        seekTo_l(int msec);
    int        prepareAsync_l();
    int        getDuration_l(int *msec);
    int        attachNewPlayer(const sp<IMediaPlayer>& player);
    int        reset_l();
    int        doSetRetransmitEndpoint(const sp<IMediaPlayer>& player);

    YtxMediaPlayer            mPlayer;
    thread_id_t                 mLockThreadId;
    Mutex                       mLock;
    Mutex                       mNotifyLock;
    Condition                   mSignal;
  //  sp<MediaPlayerListener>     mListener;
    void*                       mCookie;
  //  media_player_states         mCurrentState;
    int                         mCurrentPosition;
    int                         mSeekPosition;
    bool                        mPrepareSync;
    int                    mPrepareStatus;
  //  audio_stream_type_t         mStreamType;
    bool                        mLoop;
    float                       mLeftVolume;
    float                       mRightVolume;
    int                         mVideoWidth;
    int                         mVideoHeight;
    int                         mAudioSessionId;
    float                       mSendLevel;
  //  struct sockaddr_in          mRetransmitEndpoint;
    bool                        mRetransmitEndpointValid;


};




#endif //YTXPLAYER_YTXMEDIAPLAYER_H
