package com.ytx.ican.media.player;

/**
 * Created by Administrator on 2016/9/2.
 */

public class YtxMediaPlayerTest{


    private static native void native_init();

    private native void native_setup(Object YtxMediaPlayer_this);

    private native void native_finalize();

    private native void native_message_loop(Object YtxMediaPlayer_this);




    private native void died();


    private native void  disconnect() ;

    private native int  setDataSource(String url) ;
    private native int  setDataSource(int fd, long offset, long length) ;

    private native int  prepare() ;

    private native int  prepareAsync() ;

    private native int  start() ;

    private native int  stop() ;

    private native int  pause() ;

    private native int isPlaying() ;

    private native int  getVideoWidth() ;

    private native int  getVideoHeight() ;

    private native int  seekTo(int msec) ;

    private native int  getCurrentPosition() ;

    private native int  getDuration() ;

    private native int  reset() ;

    //    int        setAudioStreamType(audio_stream_type_t type);
    private native int   setLooping(int loop) ;

    private native int  isLooping() ;

    private native int   setVolume(float leftVolume, float rightVolume) ;

    private native int   setAudioSessionId(int sessionId) ;

    private native int   getAudioSessionId() ;

    private native int   setAuxEffectSendLevel(float level) ;

    private native int   attachAuxEffect(int effectId) ;

    private native int   setRetransmitEndpoint(String addrString, long port) ;


    private native int updateProxyConfig(String host, int  port, String exclusionList) ;

    private native void clear_l() ;

    private native int  seekTo_l(int msec) ;

    private native int  prepareAsync_l() ;

    private native int  getDuration_l() ;

    private native int  reset_l() ;

}
