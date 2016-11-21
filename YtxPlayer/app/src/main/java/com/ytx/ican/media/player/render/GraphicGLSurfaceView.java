package com.ytx.ican.media.player.render;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.renderscript.Matrix4f;
import android.util.AttributeSet;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Interpolator;

import com.ytx.ican.media.player.YtxLog;
import com.ytx.ican.media.player.gl2jni.GL2JNILib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Vector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2016/9/10.
 */

public class GraphicGLSurfaceView extends GLSurfaceView {


    public static final String TAG = "GraphicGLSurfaceView";
    public final GraphicRenderer renderer;
    public int mMaxTextureSize;
    long mAnimaStartTime;
    long mAnimaTime = 400;
    float mScale = 1.0f;
    boolean mIsFinger = false;
    float mTargeScaleOffset = 0.0f;
    float mStartScale;

    float mOffsetX = 0;
    float mOffsetY = 0;
    int mMaxOffsetX;
    int mMaxOffsetY;
    float mMiniScale;

    int mWidth;
    int mHeight;

    Picture firstPicture;
    Interpolator mInterpolator = new AccelerateDecelerateInterpolator();

    volatile boolean mIsResume = false;

    volatile boolean isInitial = false;


    public static interface PictureSnapCallback {
        void onSnap(Bitmap bitmap);
    }

    public GraphicGLSurfaceView(Context context) {
        //super(context);
        this(context,null);
    }

    public GraphicGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        if(!supportsOpenGLES2(context)){
            throw new RuntimeException("not support gles 2.0");
        }
        renderer = new GraphicRenderer();
        YtxLog.d(TAG,"GraphicGLSurfaceView 1");
        setEGLContextClientVersion(2);
        YtxLog.d(TAG,"GraphicGLSurfaceView 2");
        setEGLConfigChooser(new CustomChooseConfig2.ComponentSizeChooser(8, 8, 8, 8, 0, 0));
        YtxLog.d(TAG,"GraphicGLSurfaceView 3");
        getHolder().setFormat(PixelFormat.RGBA_8888);
        YtxLog.d(TAG,"GraphicGLSurfaceView 4");
        setRenderer(renderer);
        YtxLog.d(TAG,"GraphicGLSurfaceView 5");
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        YtxLog.d(TAG,"GraphicGLSurfaceView 6");
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
        private int mScreenWidth=720, mScreenHeight=1080;
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
            if (!prog.isProgramBuilt()) {
                prog.buildProgram();
                YtxLog.d("GLFrameRenderer","GLFrameRenderer :: buildProgram done");
            }

          //  GL2JNILib.native_init_opengl();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
           // GL2JNILib.native_resize_opengl(width,height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
          //  GL2JNILib.native_step_opengl();
                       drawFrame2();
        }

        public void drawFrame2(){

            synchronized (this) {
                if (y != null) {
                    // reset position, have to be done
                    y.position(0);
                    u.position(0);
                    v.position(0);
                    prog.buildTextures(y, u, v, 640, 272);
                  //  prog.buildTextures(y, u, v, 1280, 720);
                    GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
                    prog.drawFrame();
                }
            }
        }
        public void release() {
            RendererUtils.releaseRenderContext(renderContext);
        }

        public void snap(final PictureSnapCallback callback) {
            if (callback == null)
                return;
            if (picture == null) {
                if (callback != null) {
                    callback.onSnap(null);
                }
                return;
            }

            queue(new Runnable() {
                @Override
                public void run() {
                    Bitmap bitmap = RendererUtils.saveTexture(picture.texture(), picture.width(),
                            picture.height());
                    if (callback != null) {
                        callback.onSnap(bitmap);
                    }
                }
            });

//            Log.d(TAG, "request render");
            requestRender();

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

                if (mScreenWidth > 0 && mScreenHeight > 0) {
                    float f1 = 1f * mScreenHeight / mScreenWidth;
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

        private static final String VERTEX_SHADER = "attribute vec4 vPosition;\n" + "attribute vec2 a_texCoord;\n"
                + "varying vec2 tc;\n" + "void main() {\n" + "gl_Position = vPosition;\n" + "tc = a_texCoord;\n" + "}\n";

        private static final String FRAGMENT_SHADER = "precision mediump float;\n" + "uniform sampler2D tex_y;\n"
                + "uniform sampler2D tex_u;\n" + "uniform sampler2D tex_v;\n" + "varying vec2 tc;\n" + "void main() {\n"
                + "vec4 c = vec4((texture2D(tex_y, tc).r - 16./255.) * 1.164);\n"
                + "vec4 U = vec4(texture2D(tex_u, tc).r - 128./255.);\n"
                + "vec4 V = vec4(texture2D(tex_v, tc).r - 128./255.);\n" + "c += V * vec4(1.596, -0.813, 0, 0);\n"
                + "c += U * vec4(0, -0.392, 2.017, 0);\n" + "c.a = 1.0;\n" + "gl_FragColor = c;\n" + "}\n";



    }

    public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){
        renderer.updateYuv(ydata,udata,vdata);
        requestRender();
    }


}
