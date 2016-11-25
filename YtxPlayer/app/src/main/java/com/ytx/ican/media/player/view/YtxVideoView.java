package com.ytx.ican.media.player.view;

import android.content.Context;
import android.media.MediaFormat;
import android.util.AttributeSet;
import android.util.Pair;
import android.widget.FrameLayout;
import android.widget.MediaController;
import android.widget.VideoView;

import com.ytx.ican.media.player.YtxMediaPlayer;
import com.ytx.ican.media.player.render.VideoGlSurfaceView;


/**
 * Created by Administrator on 2016/9/19.
 */

public class YtxVideoView extends FrameLayout implements MediaController.MediaPlayerControl{

    private static final String TAG = "YtxVideoView";
    // all possible internal states
    private static final int STATE_ERROR              = -1;
    private static final int STATE_IDLE               = 0;
    private static final int STATE_PREPARING          = 1;
    private static final int STATE_PREPARED           = 2;
    private static final int STATE_PLAYING            = 3;
    private static final int STATE_PAUSED             = 4;
    private static final int STATE_PLAYBACK_COMPLETED = 5;

    //-------------------------
    // Extend: Render
    //-------------------------
    public static final int RENDER_NONE = 0;
    public static final int RENDER_SURFACE_VIEW = 1;
    public static final int RENDER_TEXTURE_VIEW = 2;

    private Context mAppContext;

    private int mVideoWidth;
    private int mVideoHeight;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private VideoView mVideoView;
    YtxMediaPlayer mPlayer;

    private int mCurrentState = STATE_IDLE;
    private int mTargetState  = STATE_IDLE;

    private VideoGlSurfaceView mSurfaceView;

    public YtxVideoView(Context context) {
        super(context);
        initVideoView(context);

    }

    public YtxVideoView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initVideoView(context);
    }

    public YtxVideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initVideoView(context);
    }


    public void initVideoView(Context context){
//        mSurfaceView = new VideoGlSurfaceViewFFMPEG(context);
//        mPlayer = new YtxMediaPlayer();
        mAppContext = context.getApplicationContext();
        mVideoWidth = 0;
        mVideoHeight = 0;

        initRenders();
        
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        mCurrentState = STATE_IDLE;
        mTargetState  = STATE_IDLE;
    }

    private void initRenders() {

        setRender(RENDER_SURFACE_VIEW);
    }

    private void setRender(int render) {
        switch (render) {
            case RENDER_NONE: {

            }
            break;
            case RENDER_TEXTURE_VIEW: {

            }
            break;

            case RENDER_SURFACE_VIEW: {
                //使用surface进行render
            }
            break;

            default: {

            }

            break;
        }
    }
//
//    public void setRenderView(IRenderView renderView){
//
//    }

    @Override
    public void start() {

    }

    @Override
    public void pause() {

    }

    @Override
    public int getDuration() {
        return 0;
    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public void seekTo(int pos) {

    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public int getBufferPercentage() {
        return 0;
    }

    @Override
    public boolean canPause() {
        return false;
    }

    @Override
    public boolean canSeekBackward() {
        return false;
    }

    @Override
    public boolean canSeekForward() {
        return false;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }
}
