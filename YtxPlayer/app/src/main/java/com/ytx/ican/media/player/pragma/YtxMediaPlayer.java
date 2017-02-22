package com.ytx.ican.media.player.pragma;

import android.content.Context;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.ytx.ican.media.player.misc.IMediaDataSource;
import com.ytx.ican.media.player.misc.ITrackInfo;
import com.ytx.ican.media.player.render.GraphicRenderer;
import com.ytx.ican.media.player.render.VideoGlSurfaceView;

import java.io.FileDescriptor;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Map;

/**
 * Created by Administrator on 2016/9/2.
 */

public class YtxMediaPlayer extends AbstractMediaPlayer {


    public static final String TAG = "YtxMediaPlayer";

    public static final int MEDIA_NOP = 0; // interface test message
    public static final int MEDIA_PREPARED = 1;
    public static final int MEDIA_PLAYBACK_COMPLETE = 2;
    public static final int MEDIA_BUFFERING_UPDATE = 3;
    public static final int MEDIA_SEEK_COMPLETE = 4;
    public static final int MEDIA_SET_VIDEO_SIZE = 5;
    public static final int MEDIA_TIMED_TEXT = 99;
    public static final int MEDIA_ERROR = 100;
    public static final int MEDIA_INFO = 200;
    public static final int MEDIA_STOPPED  = 201;


    private static final int MEDIA_PLAYER_STATE_ERROR        = 0;
    private static final int MEDIA_PLAYER_IDLE               = 1 << 0;
    private static final int MEDIA_PLAYER_INITIALIZED        = 1 << 1;
    private static final int MEDIA_PLAYER_PREPARING          = 1 << 2;
    private static final int MEDIA_PLAYER_PREPARED           = 1 << 3;
    private static final int MEDIA_PLAYER_DECODED            = 1 << 4;
    private static final int MEDIA_PLAYER_STARTED            = 1 << 5;
    private static final int MEDIA_PLAYER_PAUSED             = 1 << 6;
    private static final int MEDIA_PLAYER_STOPPED            = 1 << 7;
    private static final int MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 8;

    VideoGlSurfaceView glSurface;
    private EventHandler mEventHandler;

    private int mNativeContext; // accessed by native methods
    private int mNativeSurfaceTexture;  // accessed by native methods
    private int mListenerContext; // accessed by native methods
    private long mNativeMediaPlayer; // accessed by native methods

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

    public String getStorageDirectory(){
        return Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;
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
              //      libLoader.loadLibrary("x264");

                    libLoader.loadLibrary("charset");
                    libLoader.loadLibrary("iconv");
                    libLoader.loadLibrary("xml2");

                    libLoader.loadLibrary("png");
                 //   libLoader.loadLibrary("harfbuzz");
                    libLoader.loadLibrary("freetype");

                    libLoader.loadLibrary("fontconfig");
                    libLoader.loadLibrary("fribidi");
                    libLoader.loadLibrary("ass");


                    libLoader.loadLibrary("avutil");
                    libLoader.loadLibrary("swresample");
                    libLoader.loadLibrary("swscale");
                    libLoader.loadLibrary("postproc");
                    libLoader.loadLibrary("avcodec");
                    libLoader.loadLibrary("avformat");
                    libLoader.loadLibrary("avdevice");
                    libLoader.loadLibrary("avfilter");
                    libLoader.loadLibrary("native-ytx-lib");

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

    @Override
    public void setSurfaceView(VideoGlSurfaceView glSurface){

        //glsurface.toTexture();
        this.glSurface = glSurface;
        _setGlSurface(this.glSurface);
    }


    @Override
    public void setDisplay(SurfaceHolder sh) {

    }

    @Override
    public void setDataSource(Context context, Uri uri) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(Context context, Uri uri, Map<String, String> headers) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(FileDescriptor fd) throws IOException, IllegalArgumentException, IllegalStateException {

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
        _prepareAsync();
    }

    @Override
    public void prepare() throws IllegalStateException {
        _prepare();
    }

    @Override
    public void start() throws IllegalStateException {
        YtxLog.d(TAG,"start");
        _start();
    }

    @Override
    public void stop() throws IllegalStateException {
        YtxLog.d(TAG,"stop");
        _stop();
    }

    @Override
    public void pause() throws IllegalStateException {
        YtxLog.d(TAG,"pause");
            _pause();
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
        YtxLog.d(TAG,"isPlaying ="+_isPlaying());
        return _isPlaying()!=0;
    }

    @Override
    public void seekTo(int msec) throws IllegalStateException {
        YtxLog.d(TAG,"seekTo ="+msec);
        _seekTo(msec);
    }

    @Override
    public long getCurrentPosition() {
        return _getCurrentPosition();
    }

    @Override
    public long getDuration() {
        return _getDuration();
    }

    @Override
    public void release() {
        YtxLog.d(TAG,"release");
        _release();
    }

    @Override
    public void reset() {
        YtxLog.d(TAG,"reset");
    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    @Override
    public MediaInfo getMediaInfo() {
        return null;
    }

    @Override
    public void setLogEnabled(boolean enable) {

    }

    @Override
    public boolean isPlayable() {
        return false;
    }

    @Override
    public void setAudioStreamType(int streamtype) {

    }

    @Override
    public void setKeepInBackground(boolean keepInBackground) {

    }

    @Override
    public int getVideoSarNum() {
        return 0;
    }

    @Override
    public int getVideoSarDen() {
        return 0;
    }

    @Override
    public void setWakeMode(Context context, int mode) {

    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public boolean isLooping() {
        return false;
    }

    @Override
    public ITrackInfo[] getTrackInfo() {
        return new ITrackInfo[0];
    }

    @Override
    public void setSurface(Surface surface) {

    }

    @Override
    public void setDataSource(IMediaDataSource mediaDataSource) {

    }


    private static class EventHandler extends Handler {
        private final WeakReference<YtxMediaPlayer> mWeakPlayer;

        public EventHandler(YtxMediaPlayer mp, Looper looper) {
            super(looper);
            mWeakPlayer = new WeakReference<YtxMediaPlayer>(mp);
        }

        @Override
        public void handleMessage(Message msg) {
            YtxLog.d(TAG,"handleMessage");
            YtxMediaPlayer player = mWeakPlayer.get();
            if(player == null || player.mNativeContext == 0){
                return;
            }

            switch (msg.what) {
                case MEDIA_PREPARED:
                    YtxLog.d(TAG,"handleMessage MEDIA_PREPARED");
                    player.notifyOnPrepared();
                    break;
                case MEDIA_PLAYBACK_COMPLETE:
                    player.notifyOnCompletion();
                    break;
                case MEDIA_BUFFERING_UPDATE:
                    break;
                case MEDIA_SEEK_COMPLETE:
                    player.notifyOnSeekComplete();
                    break;
                case MEDIA_SET_VIDEO_SIZE:
                    break;
                case MEDIA_ERROR:
                    break;
                case MEDIA_INFO:
                    break;
                case MEDIA_STOPPED:
                    player.notifyOnInfo(MEDIA_STOPPED,0);
                    break;
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

    public static void postEventFromNative(Object weakThiz, int what,
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

    private native int  _release() ;

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
