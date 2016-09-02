package com.ytx.ican.media.player;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * Created by Administrator on 2016/9/2.
 */

public class YtxMediaPlayer implements IMediaPlayer {


    public static final String TAG = "YtxMediaPlayer";

    private EventHandler mEventHandler;

    YtxMediaPlayer(){
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

    @Override
    public void setDataSource(String path) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

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

    }

    @Override
    public void start() throws IllegalStateException {

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
        return 0;
    }

    @Override
    public int getVideoHeight() {
        return 0;
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

    private static native void native_init();

    private native void native_setup(Object YtxMediaPlayer_this);

    private native void native_finalize();

    private native void native_message_loop(Object YtxMediaPlayer_this);


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
}
