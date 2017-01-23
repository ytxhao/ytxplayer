package com.ytx.ican.media.player.render;

import android.opengl.GLSurfaceView;

import com.ytx.ican.media.player.gl2jni.GL2JNILib;
import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.pragma.YtxMediaPlayer;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Vector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2017/1/22.
 */

public class GraphicRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "GraphicRenderer";
    final Vector<Runnable> queue = new Vector<Runnable>();
    private GLProgram prog = new GLProgram(0);
    RendererUtils.RenderContext renderContext;
    Picture picture;


    private int mSurfaceWidth , mSurfaceHeight ;
    private int mVideoWidth, mVideoHeight;

    private int mNativeRenderContext;

    private static final int FLOAT_SIZE_BYTES = 4;
    private FloatBuffer createVerticesBuffer(float[] vertices) {

        FloatBuffer buffer = ByteBuffer
                .allocateDirect(vertices.length * FLOAT_SIZE_BYTES)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        buffer.put(vertices).position(0);
        return buffer;
    }

    void setPicture(Picture picture) {
        this.picture = picture;
    }


    public GraphicRenderer(){
        initNativeOnce();
        native_constructor_opengl(new WeakReference<GraphicRenderer>(this));
    }


    private static volatile boolean mIsNativeInitialized = false;
    private static void initNativeOnce() {
        synchronized (GraphicRenderer.class) {
            if (!mIsNativeInitialized) {
                native_init_opengl();
                mIsNativeInitialized = true;
            }
        }
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        YtxLog.d(TAG,"#### #### onSurfaceCreated=");
        native_create_opengl();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        native_resize_opengl(width,height);
        YtxLog.d(TAG,"#### #### onSurfaceChanged="+width+" height="+height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        //  YtxLog.d(TAG,"#### #### onDrawFrame=");
        native_step_opengl();

    }

    public void release() {
        RendererUtils.releaseRenderContext(renderContext);
    }



    public  static native void native_init_opengl();
    public  native void native_resize_opengl(int width, int height);
    public  native void native_step_opengl();
    public  native void native_create_opengl();
    public  native void native_constructor_opengl(Object GraphicRenderer_this);

}




