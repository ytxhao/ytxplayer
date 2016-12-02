package com.ytx.ican.media.player.render;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.PixelFormat;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Interpolator;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.gl2jni.GL2JNILib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Vector;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2016/9/10.
 */

public class GraphicGLSurfaceView extends GLSurfaceView {

    public static final String TAG = "GraphicGLSurfaceView";
    public GraphicRenderer renderer;

    int mWidth;
    int mHeight;

    Picture firstPicture;
    Interpolator mInterpolator = new AccelerateDecelerateInterpolator();

    volatile boolean mIsResume = false;

    volatile boolean isInitial = false;
    private ISurfaceCallback mSurfaceCallback;

    public ISurfaceCallback getSurfaceCallback() {
        return mSurfaceCallback;
    }

    public void setSurfaceCallback(ISurfaceCallback mSurfaceCallback) {
        this.mSurfaceCallback = mSurfaceCallback;
    }



    public GraphicGLSurfaceView(Context context) {
        this(context,null);
    }

    public GraphicGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    private void initView(Context context){
        if(!supportsOpenGLES2(context)){
            throw new RuntimeException("not support gles 2.0");
        }
        renderer = new GraphicRenderer();
        setEGLContextClientVersion(2);
        setEGLConfigChooser(new CustomChooseConfig2.ComponentSizeChooser(8, 8, 8, 8, 0, 0));
        // setEGLConfigChooser(new ConfigChooser(5, 6, 5, 0, 0, 0));
        getHolder().setFormat(PixelFormat.RGBA_8888);
        getHolder().addCallback(this);
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        YtxLog.d(TAG,"#### #### onMeasure getHeight=" + getHeight() +" getWidth="+getWidth());
    }


    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        YtxLog.d(TAG,"#### #### onLayout getHeight=" + getHeight() +" getWidth="+getWidth());

    }

    private boolean supportsOpenGLES2(final Context context) {
        final ActivityManager activityManager = (ActivityManager)
                context.getSystemService(Context.ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo =
                activityManager.getDeviceConfigurationInfo();
        return configurationInfo.reqGlEsVersion >= 0x20000;
    }

    public interface OnScreenWindowChangedListener {
        void onScreenWindowChanged(boolean isFinger, int width, int height, int x1, int y1, int x2, int y2);
    }

    private OnScreenWindowChangedListener onScreenWindowChangedListener = null;

    public void setOnScreenWindowChangedListener(OnScreenWindowChangedListener listener){
        onScreenWindowChangedListener = listener;
    }

    public void queue(Runnable r) {
        renderer.queue.add(r);
        requestRender();
    }

    @Override
    public void onResume() {
        super.onResume();
        mIsResume = true;
        YtxLog.d(TAG,"onResume yuhaoo isInitial="+isInitial);

//        queue(new Runnable() {
//            @Override
//            public void run() {
//                if (!isInitial) {
//                    isInitial = true;
//                    initial();
//                }
//            }
//        });
    }


    @Override
    public void onPause() {
        super.onPause();
        mIsResume = false;
    }


    protected void initial() {
        YtxLog.d(TAG, "initial");
    }

    protected void release() {
        YtxLog.d(TAG, "release");
        renderer.release();
        if (firstPicture != null) {
            firstPicture.clear();
        }
    }


    public void drawFrame() {

    }

    public void setPicture(Picture picture) {
        renderer.setPicture(picture);
        mWidth = picture.width();
        mHeight = picture.height();
    }

    class GraphicRenderer implements Renderer{

        // Vector与数组最大区别在于，数组对象创建之后长度就不能改变了，而Vector的存储空间可扩充
        final Vector<Runnable> queue = new Vector<Runnable>();
        private GLProgram prog = new GLProgram(0);
        RendererUtils.RenderContext renderContext;
        Picture picture;

        private ByteBuffer y;
        private ByteBuffer u;
        private ByteBuffer v;
        private int mSurfaceWidth , mSurfaceHeight ;
        private int mVideoWidth, mVideoHeight;

        private static final int FLOAT_SIZE_BYTES = 4;
        private  FloatBuffer createVerticesBuffer(float[] vertices) {

            FloatBuffer buffer = ByteBuffer
                    .allocateDirect(vertices.length * FLOAT_SIZE_BYTES)
                    .order(ByteOrder.nativeOrder()).asFloatBuffer();
            buffer.put(vertices).position(0);
            return buffer;
        }

        void setPicture(Picture picture) {
            this.picture = picture;
        }


        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//            if (!prog.isProgramBuilt()) {
//                prog.buildProgram();
//                YtxLog.d("GLFrameRenderer","GLFrameRenderer :: buildProgram done");
//            }
            YtxLog.d(TAG,"#### #### onSurfaceCreated=");
            GL2JNILib.native_init_opengl();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            mSurfaceWidth = width;
            mSurfaceHeight = height;
            GL2JNILib.native_resize_opengl(width,height);
            YtxLog.d(TAG,"#### #### onSurfaceChanged="+width+" height="+height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
          //  YtxLog.d(TAG,"#### #### onDrawFrame=");
            GL2JNILib.native_step_opengl();
              //         drawFrame2();
        }

        public void drawFrame2(){

            synchronized (this) {
                if (y != null) {
                    // reset position, have to be done
                    y.position(0);
                    u.position(0);
                    v.position(0);
                    prog.buildTextures(y, u, v, 640, 272);
                    GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
                    prog.drawFrame();
                }
            }
        }
        public void release() {
            RendererUtils.releaseRenderContext(renderContext);
        }



        /**
         * this method will be called from native code, it happens when the video is about to play or
         * the video size changes.
         */
        public void update(int w, int h) {
            YtxLog.d("GLFrameRenderer","INIT E");
            if (w > 0 && h > 0) {
                // 调整比例
               // prog.createBuffers(GLProgram.squareVertices);

                if (mSurfaceWidth > 0 && mSurfaceHeight > 0) {
                    float f1 = 1f * mSurfaceHeight / mSurfaceWidth;
                    float f2 = 1f * h / w;
                    if (f1 == f2) {
                        prog.createBuffers(GLProgram.squareVertices);
                    } else if (f1 < f2) {
                        float widScale = f1 / f2;
                        prog.createBuffers(new float[] { -widScale, -1.0f, widScale, -1.0f, -widScale, 1.0f, widScale,
                                1.0f, });
                    } else {
                        float heightScale = f2 / f1;
                        prog.createBuffers(new float[] { -1.0f, -heightScale, 1.0f, -heightScale, -1.0f, heightScale, 1.0f,
                                heightScale, });
                    }
                }

                // 初始化容器
                if (w != mVideoWidth && h != mVideoHeight) {
                    this.mVideoWidth = w;
                    this.mVideoHeight = h;
                    int yarraySize = w * h;
                    int uvarraySize = yarraySize / 4;
                }
            }

            YtxLog.d("GLFrameRenderer","INIT X");
        }

        public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){
                update(640,272); //显示视频区域宽高
                synchronized (this) {

                    if(y == null){
                        y = ByteBuffer.allocate(ydata.length);
                    }
                    if(u == null){
                        u = ByteBuffer.allocate(udata.length);
                    }
                    if(v == null){
                        v = ByteBuffer.allocate(vdata.length);
                    }

                    y.clear();
                    u.clear();
                    v.clear();
                    y.put(ydata, 0, ydata.length);
                    u.put(udata, 0, udata.length);
                    v.put(vdata, 0, vdata.length);
                }
        }
    }

    public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){
        renderer.updateYuv(ydata,udata,vdata);
        requestRender();
    }



    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        /* This EGL config specification is used to specify 2.0 rendering.
         * We use a minimum size of 4 bits for red/green/blue, but will
         * perform actual matching in chooseConfig() below.
         */
        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs2 =
                {
                        EGL10.EGL_RED_SIZE, 4,
                        EGL10.EGL_GREEN_SIZE, 4,
                        EGL10.EGL_BLUE_SIZE, 4,
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL10.EGL_NONE
                };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            /* Get the number of minimally matching EGL configurations
             */
            int[] num_config = new int[1];
            egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            /* Allocate then read the array of minimally matching EGL configs
             */
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);
//
//            if (DEBUG) {
//                printConfigs(egl, display, configs);
//            }
            /* Now return the "best" one
             */
            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                      EGLConfig[] configs) {
            for(EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

                // We need at least mDepthSize and mStencilSize bits
                if (d < mDepthSize || s < mStencilSize)
                    continue;

                // We want an *exact* match for red/green/blue/alpha
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                        EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                        EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                    return config;
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                     EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        private void printConfigs(EGL10 egl, EGLDisplay display,
                                  EGLConfig[] configs) {
            int numConfigs = configs.length;
            Log.w(TAG, String.format("%d configurations", numConfigs));
            for (int i = 0; i < numConfigs; i++) {
                Log.w(TAG, String.format("Configuration %d:\n", i));
                printConfig(egl, display, configs[i]);
            }
        }

        private void printConfig(EGL10 egl, EGLDisplay display,
                                 EGLConfig config) {
            int[] attributes = {
                    EGL10.EGL_BUFFER_SIZE,
                    EGL10.EGL_ALPHA_SIZE,
                    EGL10.EGL_BLUE_SIZE,
                    EGL10.EGL_GREEN_SIZE,
                    EGL10.EGL_RED_SIZE,
                    EGL10.EGL_DEPTH_SIZE,
                    EGL10.EGL_STENCIL_SIZE,
                    EGL10.EGL_CONFIG_CAVEAT,
                    EGL10.EGL_CONFIG_ID,
                    EGL10.EGL_LEVEL,
                    EGL10.EGL_MAX_PBUFFER_HEIGHT,
                    EGL10.EGL_MAX_PBUFFER_PIXELS,
                    EGL10.EGL_MAX_PBUFFER_WIDTH,
                    EGL10.EGL_NATIVE_RENDERABLE,
                    EGL10.EGL_NATIVE_VISUAL_ID,
                    EGL10.EGL_NATIVE_VISUAL_TYPE,
                    0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
                    EGL10.EGL_SAMPLES,
                    EGL10.EGL_SAMPLE_BUFFERS,
                    EGL10.EGL_SURFACE_TYPE,
                    EGL10.EGL_TRANSPARENT_TYPE,
                    EGL10.EGL_TRANSPARENT_RED_VALUE,
                    EGL10.EGL_TRANSPARENT_GREEN_VALUE,
                    EGL10.EGL_TRANSPARENT_BLUE_VALUE,
                    0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
                    0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
                    0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
                    0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
                    EGL10.EGL_LUMINANCE_SIZE,
                    EGL10.EGL_ALPHA_MASK_SIZE,
                    EGL10.EGL_COLOR_BUFFER_TYPE,
                    EGL10.EGL_RENDERABLE_TYPE,
                    0x3042 // EGL10.EGL_CONFORMANT
            };
            String[] names = {
                    "EGL_BUFFER_SIZE",
                    "EGL_ALPHA_SIZE",
                    "EGL_BLUE_SIZE",
                    "EGL_GREEN_SIZE",
                    "EGL_RED_SIZE",
                    "EGL_DEPTH_SIZE",
                    "EGL_STENCIL_SIZE",
                    "EGL_CONFIG_CAVEAT",
                    "EGL_CONFIG_ID",
                    "EGL_LEVEL",
                    "EGL_MAX_PBUFFER_HEIGHT",
                    "EGL_MAX_PBUFFER_PIXELS",
                    "EGL_MAX_PBUFFER_WIDTH",
                    "EGL_NATIVE_RENDERABLE",
                    "EGL_NATIVE_VISUAL_ID",
                    "EGL_NATIVE_VISUAL_TYPE",
                    "EGL_PRESERVED_RESOURCES",
                    "EGL_SAMPLES",
                    "EGL_SAMPLE_BUFFERS",
                    "EGL_SURFACE_TYPE",
                    "EGL_TRANSPARENT_TYPE",
                    "EGL_TRANSPARENT_RED_VALUE",
                    "EGL_TRANSPARENT_GREEN_VALUE",
                    "EGL_TRANSPARENT_BLUE_VALUE",
                    "EGL_BIND_TO_TEXTURE_RGB",
                    "EGL_BIND_TO_TEXTURE_RGBA",
                    "EGL_MIN_SWAP_INTERVAL",
                    "EGL_MAX_SWAP_INTERVAL",
                    "EGL_LUMINANCE_SIZE",
                    "EGL_ALPHA_MASK_SIZE",
                    "EGL_COLOR_BUFFER_TYPE",
                    "EGL_RENDERABLE_TYPE",
                    "EGL_CONFORMANT"
            };
            int[] value = new int[1];
            for (int i = 0; i < attributes.length; i++) {
                int attribute = attributes[i];
                String name = names[i];
                if ( egl.eglGetConfigAttrib(display, config, attribute, value)) {
                    Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
                } else {
                    // Log.w(TAG, String.format("  %s: failed\n", name));
                    while (egl.eglGetError() != EGL10.EGL_SUCCESS);
                }
            }
        }

        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }


//    private static final class SurfaceCallback implements SurfaceHolder.Callback {
//        private SurfaceHolder mSurfaceHolder;
//        private boolean mIsFormatChanged;
//        private int mFormat;
//        private int mWidth;
//        private int mHeight;
//
//
//        @Override
//        public void surfaceCreated(SurfaceHolder holder) {
//            mSurfaceHolder = holder;
//            mIsFormatChanged = false;
//            mFormat = 0;
//            mWidth = 0;
//            mHeight = 0;
//        }
//
//        @Override
//        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//            mSurfaceHolder = holder;
//            mIsFormatChanged = true;
//            mFormat = format;
//            mWidth = width;
//            mHeight = height;
//        }
//
//        @Override
//        public void surfaceDestroyed(SurfaceHolder holder) {
//            mSurfaceHolder = null;
//            mIsFormatChanged = false;
//            mFormat = 0;
//            mWidth = 0;
//            mHeight = 0;
//        }
//    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        super.surfaceCreated(holder);
        YtxLog.d(TAG,"#### #### surfaceCreated getHeight=" + getHeight() +" getWidth="+getWidth());
        if(mSurfaceCallback != null){
            mSurfaceCallback.onSurfaceCreated(holder);
        }
    }


    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        super.surfaceChanged(holder, format, w, h);
        YtxLog.d(TAG,"#### #### surfaceChanged getHeight=" + getHeight() +" getWidth="+getWidth());
        if(mSurfaceCallback != null){
            mSurfaceCallback.onSurfaceChanged(holder,  format,  w,  h);
        }
    }


    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        YtxLog.d(TAG,"#### #### surfaceDestroyed getHeight=" + getHeight() +" getWidth="+getWidth());
        if(mSurfaceCallback != null){
            mSurfaceCallback.onSurfaceDestroyed(holder);
        }
    }

    public interface ISurfaceCallback {
        /**
         * @param holder

         */
        void onSurfaceCreated(@NonNull SurfaceHolder holder);

        /**
         * @param holder
         * @param format could be 0
         * @param width
         * @param height
         */
        void onSurfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height);

        void onSurfaceDestroyed(@NonNull SurfaceHolder holder);
    }
}
