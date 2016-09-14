package com.ytx.ican.media.player.test;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.ytx.ican.media.player.YtxLog;

/**
 * Created by Administrator on 2016/9/12.
 */

public class GLFrameSurface extends GLSurfaceView {
    public GLFrameSurface(Context context, AttributeSet attrs) {
        super(context, attrs);
    }


    @Override
    protected void onAttachedToWindow() {
        YtxLog.d("GLFrameSurface","surface onAttachedToWindow()");
        super.onAttachedToWindow();
        // setRenderMode() only takes effectd after SurfaceView attached to window!
        // note that on this mode, surface will not render util GLSurfaceView.requestRender() is
        // called, it's good and efficient -v-
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        YtxLog.d("GLFrameSurface","surface setRenderMode RENDERMODE_WHEN_DIRTY");
    }
}
