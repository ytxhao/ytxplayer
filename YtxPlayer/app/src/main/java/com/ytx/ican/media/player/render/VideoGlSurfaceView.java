package com.ytx.ican.media.player.render;

import android.content.Context;
import android.util.AttributeSet;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Created by Administrator on 2016/9/12.
 */

public class VideoGlSurfaceView extends GraphicGLSurfaceView {

    public VideoGlSurfaceView(Context context) {
        super(context);
    }

    public VideoGlSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }



    @Override
    protected void initial() {
        super.initial();
    }

    @Override
    protected void release() {
        super.release();
    }






    public void requestRender(){
        super.requestRender();
    }


    public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){
        super.updateYuv(ydata,udata,vdata);
    }
}
