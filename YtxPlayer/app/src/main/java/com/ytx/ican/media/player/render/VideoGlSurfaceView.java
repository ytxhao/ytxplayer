package com.ytx.ican.media.player.render;

import android.content.Context;
import android.util.AttributeSet;

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
