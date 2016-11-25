package com.ytx.ican.media.player.view;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.FrameLayout;

import com.ytx.ican.media.player.render.VideoGlSurfaceViewFFMPEG;

/**
 * Created by Administrator on 2016/9/19.
 */

public class YtxVideoView extends FrameLayout {

    private Context mAppContext;

    private int mVideoWidth;
    private int mVideoHeight;
    private int mSurfaceWidth;
    private int mSurfaceHeight;

    private VideoGlSurfaceViewFFMPEG mSurfaceView;

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
        mSurfaceView = new VideoGlSurfaceViewFFMPEG(context);
        mAppContext = context.getApplicationContext();
        mVideoWidth = 0;
        mVideoHeight = 0;
    }
}
