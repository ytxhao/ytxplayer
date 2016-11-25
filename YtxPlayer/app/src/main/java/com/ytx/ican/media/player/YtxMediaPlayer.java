package com.ytx.ican.media.player;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.ytx.ican.media.player.render.VideoGlSurfaceView;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * Created by Administrator on 2016/9/2.
 */

public class YtxMediaPlayer implements IMediaPlayer {


    public static final String TAG = "YtxMediaPlayer";

    VideoGlSurfaceView glSurface;
    private EventHandler mEventHandler;

    private int mNativeContext; // accessed by native methods
    private int mNativeSurfaceTexture;  // accessed by native methods
    private int mListenerContext; // accessed by native methods

    public YtxMediaPlayer(){
        this(sLocalLibLoader);
    }

    public YtxMediaPlayer(LibLoader libLoader){
        initPlayer(libLoader);
    }

    private void initPlayer(LibLoader libLoader){
        loadLibrariesOnce(libLoader);
        initNativeOnce();

        Looper looper;

        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }

        /*
         * Native setup requires a weak reference to our object. It's easier to
         * create it here than in C++.
         */
        native_setup(new WeakReference<YtxMediaPlayer>(this));
    }


    private static final LibLoader sLocalLibLoader = new LibLoader() {
        @Override
        public void loadLibrary(String libName) throws UnsatisfiedLinkError, SecurityException {
            System.loadLibrary(libName);
        }
    };
    private static volatile boolean mIsLibLoaded = false;
    public static void loadLibrariesOnce(LibLoader libLoader) {
        synchronized (YtxMediaPlayer.class) {
            if (!mIsLibLoaded) {
                if (libLoader == null)
                    libLoader = sLocalLibLoader;

                    libLoader.loadLibrary("gnustl_shared");
                    libLoader.loadLibrary("mp4v2");
                    libLoader.loadLibrary("faad");
                    libLoader.loadLibrary("faac");
                    libLoader.loadLibrary("rtmp");
                    libLoader.loadLibrary("x264");
                    libLoader.loadLibrary("avutil");
                    libLoader.loadLibrary("swresample");
                    libLoader.loadLibrary("swscale");
                    libLoader.loadLibrary("postproc");
                    libLoader.loadLibrary("avcodec");
                    libLoader.loadLibrary("avformat");
                    libLoader.loadLibrary("avdevice");
                    libLoader.loadLibrary("avfilter");

                    mIsLibLoaded = true;
            }
        }
    }


    private static volatile boolean mIsNativeInitialized = false;
    private static void initNativeOnce() {
        synchronized (YtxMediaPlayer.class) {
            if (!mIsNativeInitialized) {
                native_init();
                mIsNativeInitialized = true;
            }
        }
    }


    public void setSurfaceView(VideoGlSurfaceView glSurface){

        //glsurface.toTexture();
        this.glSurface = glSurface;
        _setGlSurface(this.glSurface);
    }


    @Override
    public void setDataSource(String path) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {
        _setDataSource(path);
    }

    @Override
    public String getDataSource() {
        return null;
    }

    @Override
    public void prepareAsync() throws IllegalStateException {

    }

    @Override
    public void prepare() throws IllegalStateException {
        _prepare();
    }

    @Override
    public void start() throws IllegalStateException {
        _start();
    }

    @Override
    public void stop() throws IllegalStateException {

    }

    @Override
    public void pause() throws IllegalStateException {

    }

    @Override
    public void setScreenOnWhilePlaying(boolean screenOn) {

    }

    @Override
    public int getVideoWidth() {
        return _getVideoWidth();
    }

    @Override
    public int getVideoHeight() {
        return _getVideoHeight();
    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public void seekTo(long msec) throws IllegalStateException {

    }

    @Override
    public long getCurrentPosition() {
        return 0;
    }

    @Override
    public long getDuration() {
        return 0;
    }

    @Override
    public void release() {

    }

    @Override
    public void reset() {

    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }


    private static class EventHandler extends Handler {
        private final WeakReference<YtxMediaPlayer> mWeakPlayer;

        public EventHandler(YtxMediaPlayer mp, Looper looper) {
            super(looper);
            mWeakPlayer = new WeakReference<YtxMediaPlayer>(mp);
        }

        @Override
        public void handleMessage(Message msg) {
//            IjkMediaPlayer player = mWeakPlayer.get();
//            if (player == null || player.mNativeMediaPlayer == 0) {
//                YtxLog.w(TAG,
//                        "IjkMediaPlayer went away with unhandled events");
//                return;
//            }

            switch (msg.what) {


                default:
                    YtxLog.e(TAG, "Unknown message type " + msg.what);
            }
        }
    }


    /*
 * Called from native code when an interesting event happens. This method
 * just uses the EventHandler system to post the event back to the main app
 * thread. We use a weak reference to the original IjkMediaPlayer object so
 * that the native code is safe from the object disappearing from underneath
 * it. (This is the cookie passed to native_setup().)
 */

    private static void postEventFromNative(Object weakThiz, int what,
                                            int arg1, int arg2, Object obj) {
        if (weakThiz == null)
            return;

        @SuppressWarnings("rawtypes")
        YtxMediaPlayer mp = (YtxMediaPlayer) ((WeakReference) weakThiz).get();
        if (mp == null) {
            return;
        }

//        if (what == MEDIA_INFO && arg1 == MEDIA_INFO_STARTED_AS_NEXT) {
//            // this acquires the wakelock if needed, and sets the client side
//            // state
//            mp.start();
//        }
        if (mp.mEventHandler != null) {
            Message m = mp.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mp.mEventHandler.sendMessage(m);
        }
    }


    private static native void native_init();

    private native void native_setup(Object YtxMediaPlayer_this);

    private native void native_finalize();

    private native void native_message_loop(Object YtxMediaPlayer_this);


    private native void _setGlSurface(Object glSurface);

    private native void _died();


    private native void  _disconnect() ;

    private native int  _setDataSource(String url) ;
    private native int  _setDataSource(int fd, long offset, long length) ;

    private native int  _prepare() ;

    private native int  _prepareAsync() ;

    private native int  _start() ;

    private native int  _stop() ;

    private native int  _pause() ;

    private native int _isPlaying() ;

    private native int  _getVideoWidth() ;

    private native int  _getVideoHeight() ;

    private native int  _seekTo(int msec) ;

    private native int  _getCurrentPosition() ;

    private native int  _getDuration() ;

    private native int  _reset() ;

    //    int        setAudioStreamType(audio_stream_type_t type);
    private native int   _setLooping(int loop) ;

    private native int  _isLooping() ;

    private native int   _setVolume(float leftVolume, float rightVolume) ;

    private native int   _setAudioSessionId(int sessionId) ;

    private native int   _getAudioSessionId() ;

    private native int   _setAuxEffectSendLevel(float level) ;

    private native int   _attachAuxEffect(int effectId) ;

    private native int   _setRetransmitEndpoint(String addrString, long port) ;


    private native int _updateProxyConfig(String host, int  port, String exclusionList) ;

    private native void _clear_l() ;

    private native int  _seekTo_l(int msec) ;

    private native int  _prepareAsync_l() ;

    private native int  _getDuration_l() ;

    private native int  _reset_l() ;

}
