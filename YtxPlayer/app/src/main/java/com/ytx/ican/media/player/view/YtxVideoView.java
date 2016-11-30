package com.ytx.ican.media.player.view;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.Configuration;
import android.media.MediaFormat;
import android.net.Uri;
import android.os.Build;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Pair;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.MediaController;
import android.widget.VideoView;

import com.ytx.ican.media.player.YtxMediaPlayer;
import com.ytx.ican.media.player.render.GraphicGLSurfaceView;
import com.ytx.ican.media.player.render.VideoGlSurfaceView;

import java.io.IOException;
import java.util.Map;


/**
 * Created by Administrator on 2016/9/19.
 */

public class YtxVideoView extends FrameLayout implements MediaController.MediaPlayerControl, View.OnTouchListener, GraphicGLSurfaceView.OnScreenWindowChangedListener {

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
    private int mScreenWidth;
    private int mScreenHeight;
    private VideoView mVideoView;
    YtxMediaPlayer mPlayer;

    public VideoGlSurfaceView mGlSurface = null;
    private int mCurrentState = STATE_IDLE;
    private int mTargetState  = STATE_IDLE;


    // settable by the client
    private Uri         mUri;
    private Map<String, String> mHeaders;
    private int         mSeekWhenPrepared;  // recording the seek position while preparing
    Context mContext;
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

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public YtxVideoView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        initVideoView(context);
    }

    public void initVideoView(Context context){
//        mSurfaceView = new VideoGlSurfaceViewFFMPEG(context);
//        mPlayer = new YtxMediaPlayer();
        mAppContext = context.getApplicationContext();
        mVideoWidth = 0;
        mVideoHeight = 0;

        initRenders();
        initSurface(context);

        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        mCurrentState = STATE_IDLE;
        mTargetState  = STATE_IDLE;
    }

    private void initSurface(Context context) {
        mGlSurface = new VideoGlSurfaceView(context);

        mGlSurface.setOnTouchListener(this);
        mGlSurface.setOnScreenWindowChangedListener(this);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);
        mGlSurface.setLayoutParams(lp);
        addView(mGlSurface);
        mGlSurface.setSurfaceCallback(mSurfaceCallback);
        WindowManager wm = (WindowManager) getContext().getSystemService(Context.WINDOW_SERVICE);
        DisplayMetrics outMetrics = new DisplayMetrics();
        wm.getDefaultDisplay().getMetrics(outMetrics);
        mScreenWidth = outMetrics.widthPixels;
        mScreenHeight = outMetrics.heightPixels;
        Configuration configuration = getResources().getConfiguration();
        if (configuration.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            layOutLandscape();
        } else {
            layOutPortrait();
        }
    }

    private void layOutPortrait() {

    }

    private void layOutLandscape() {
        
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

    public void setVideoPath(String path) {
        setVideoURI(Uri.parse(path));
    }

    public void setVideoURI(Uri uri) {
        setVideoURI(uri, null);
    }

    public void setVideoURI(Uri uri, Map<String, String> headers) {
        mUri = uri;
        mHeaders = headers;
        mSeekWhenPrepared = 0;
        openVideo();
        requestLayout();
        invalidate();
    }

    @Override
    public void start() {
        mPlayer.start();
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


    @Override
    public boolean onTouch(View v, MotionEvent event) {
        return false;
    }

    @Override
    public void onScreenWindowChanged(boolean isFinger, int width, int height, int x1, int y1, int x2, int y2) {

    }


    private GraphicGLSurfaceView.ISurfaceCallback mSurfaceCallback = new GraphicGLSurfaceView.ISurfaceCallback() {
        @Override
        public void onSurfaceCreated(@NonNull SurfaceHolder holder) {
               if(mPlayer == null){
                   openVideo();
               }

        }

        @Override
        public void onSurfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

        }

        @Override
        public void onSurfaceDestroyed(@NonNull SurfaceHolder holder) {

        }
    };

    private void openVideo() {

        if(mUri == null){
            return;
        }
        mPlayer = new YtxMediaPlayer();
        mPlayer.setSurfaceView(mGlSurface);
        try {
            mPlayer.setDataSource(mUri.toString());
        } catch (IOException e) {
            e.printStackTrace();
        }
        mPlayer.prepare();

    }

}
