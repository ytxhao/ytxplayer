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

        private FloatBuffer texVertices;
        private FloatBuffer verVertices;

        int ATTRIB_VERTEX  = 3;
        int ATTRIB_TEXTURE = 4;
        int p;
        int[] id_y = new int[1];
        int[] id_u = new int[1];
        int[] id_v = new int[1];
        private int _ytid = -1, _utid = -1, _vtid = -1;
       // int id_y, id_u, id_v; // Texture id
        int textureUniformY, textureUniformU,textureUniformV;
        int pixel_w = 640, pixel_h = 272;

        final Vector<Runnable> queue = new Vector<Runnable>();
        private GLProgram prog = new GLProgram(0);
        RendererUtils.RenderContext renderContext;
        Picture picture;

        int viewWidth;
        int viewHeight;
        int lastWidth, lastHeight, lastX1, lastY1, lastX2, lastY2;

        private ByteBuffer y;
        private ByteBuffer u;
        private ByteBuffer v;
        private int mScreenWidth=720, mScreenHeight=1080;
        private int mVideoWidth, mVideoHeight;

        private static final int FLOAT_SIZE_BYTES = 4;
        private  FloatBuffer createVerticesBuffer(float[] vertices) {
            // if (vertices.length != 8) {
            // throw new RuntimeException("Number of vertices should be four.");
            // }

            FloatBuffer buffer = ByteBuffer
                    .allocateDirect(vertices.length * FLOAT_SIZE_BYTES)
                    .order(ByteOrder.nativeOrder()).asFloatBuffer();
            buffer.put(vertices).position(0);
            return buffer;
        }


        void InitShaders(){
            // create shaders
            int v, f;
//            int vertexShader = GLES20.glShaderSource(GLES20.GL_VERTEX_SHADER, source);
            //    int pixelShader = GLES20.glShaderSource(GLES20.GL_FRAGMENT_SHADER, source);
            //Shader: step1
            v = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
            f = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
            //Shader: step2
            GLES20.glShaderSource(v,VERTEX_SHADER);
            GLES20.glShaderSource(f, FRAGMENT_SHADER);

            //Shader: step3
            GLES20.glCompileShader(v);
            //Debug
            int[] compiledv = new int[1];
            GLES20.glGetShaderiv(v, GLES20.GL_COMPILE_STATUS, compiledv,0);

            GLES20.glCompileShader(f);
            int[] compiledf = new int[1];
            GLES20.glGetShaderiv(f, GLES20.GL_COMPILE_STATUS, compiledf,0);

            //Program: Step1
            p = GLES20.glCreateProgram();
            //Program: Step2
            GLES20.glAttachShader(p,v);
            GLES20.glAttachShader(p,f);

            GLES20.glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");
            GLES20.glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn");

            //Program: Step3
            GLES20.glLinkProgram(p);
            //Debug
            int[] linkStatus = new int[1];
            GLES20.glGetProgramiv(p, GLES20.GL_LINK_STATUS,linkStatus,0);
            //Program: Step4
            GLES20.glUseProgram(p);


            //Get Uniform Variables Location
            textureUniformY = GLES20.glGetUniformLocation(p, "tex_y");
            textureUniformU = GLES20.glGetUniformLocation(p, "tex_u");
            textureUniformV = GLES20.glGetUniformLocation(p, "tex_v");

            final float vertexVertices[] = {
                    -1.0f, -1.0f,
                    1.0f, -1.0f,
                    -1.0f,  1.0f,
                    1.0f,  1.0f,
            };

            final float textureVertices[] = {
                    0.0f,  1.0f,
                    1.0f,  1.0f,
                    0.0f,  0.0f,
                    1.0f,  0.0f,
            };

            texVertices = createVerticesBuffer(textureVertices);
            verVertices = createVerticesBuffer(vertexVertices);

            //Set Arrays
            GLES20.glVertexAttribPointer(ATTRIB_VERTEX, 2, GLES20.GL_FLOAT, false, 0, verVertices);
            //Enable it
            GLES20.glEnableVertexAttribArray(ATTRIB_VERTEX);
            GLES20.glVertexAttribPointer(ATTRIB_TEXTURE, 2, GLES20.GL_FLOAT, false, 0, texVertices);
            GLES20.glEnableVertexAttribArray(ATTRIB_TEXTURE);


            //Init Texture
            GLES20.glGenTextures(1, id_y,0);
            _ytid = id_y[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _ytid);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MAG_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

            GLES20.glGenTextures(1, id_u,0);
            _utid = id_u[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _utid);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MAG_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

            GLES20.glGenTextures(1, id_v,0);
            _vtid = id_v[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _vtid);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MAG_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        }


        void setPicture(Picture picture) {
            this.picture = picture;
        }

        void setRenderMatrix(float[] matrix) {
            RendererUtils.setRenderMatrix(renderContext, matrix);
        }

        void setRenderMatrix(int srcWidth, int srcHeight) {

//            int srcWidth = photo.width();
//            int srcHeight = photo.height();

            Matrix4f matrix4f = new Matrix4f();
            float srcAspectRatio = ((float) srcWidth) / srcHeight;
            float dstAspectRatio = ((float) viewWidth) / viewHeight;
            float relativeAspectRatio = dstAspectRatio / srcAspectRatio;
            float ratioscale = 1.0f;
            float x, y;
            float xScale, yScale;
            if (relativeAspectRatio < 1.0f) {
                ratioscale = srcAspectRatio / dstAspectRatio;
                mMiniScale = relativeAspectRatio;

                mMaxOffsetX = (int) (viewWidth * ratioscale * mScale - viewWidth);
                mMaxOffsetY = (int) (viewHeight * mScale - viewHeight);
                if (mOffsetX < -mMaxOffsetX) {
                    mOffsetX = -mMaxOffsetX;
                }
                if (mOffsetX > mMaxOffsetX) {
                    mOffsetX = mMaxOffsetX;
                }
                if (mOffsetY < -mMaxOffsetY) {
                    mOffsetY = -mMaxOffsetY;
                }
                if (mOffsetY > mMaxOffsetY) {
                    mOffsetY = mMaxOffsetY;
                }

                xScale = ratioscale * mScale;
                yScale = mScale;
                matrix4f.scale(xScale, yScale, 0);
                x = mOffsetX / (viewWidth * xScale);
                y = mOffsetY / (viewHeight * yScale);
                if (mScale < 1.0) {
                    y = 0.0f;
                }
                matrix4f.translate(x, y, 0);
            } else {
                mMiniScale = 1.0f;
                ratioscale = relativeAspectRatio;

                mMaxOffsetX = (int) (viewWidth * mScale - viewWidth);
                mMaxOffsetY = (int) (viewHeight * ratioscale * mScale - viewHeight);
                if (mOffsetX < -mMaxOffsetX) {
                    mOffsetX = -mMaxOffsetX;
                }
                if (mOffsetX > mMaxOffsetX) {
                    mOffsetX = mMaxOffsetX;
                }
                if (mOffsetY < -mMaxOffsetY) {
                    mOffsetY = -mMaxOffsetY;
                }
                if (mOffsetY > mMaxOffsetY) {
                    mOffsetY = mMaxOffsetY;
                }

                xScale = mScale;
                yScale = ratioscale * mScale;
                matrix4f.scale(xScale, yScale, 0);
                x = mOffsetX / (viewWidth * xScale);
                y = mOffsetY / (viewHeight * yScale);
                matrix4f.translate(x, y, 0);
            }

            renderContext.mModelViewMat = matrix4f.getArray();


            // 计算看视频窗口在图像本身的矩形坐标
            int x1, y1, x2, y2;
            x1 = (int) ((1 - 1/xScale - x) * srcWidth/2);
            x2 = (int) (x1 + 1/xScale * srcWidth);
            y1 = (int) ((1/yScale - 1 - y) * srcHeight/2);
            y2 = (int) (y1 - 1/yScale * srcHeight);

            // 0<=x1<x2<=srcWidth; 0>=y1>y2>=(-srcHeight);
            if(x1 < 0) x1 = 0;
            if(x2 > srcWidth) x2 = srcWidth;
            if(y1 > 0) y1 = 0;
            if(y2 < (0 - srcHeight)) y2 = (0 - srcHeight);


            if(lastWidth != srcWidth || lastHeight != srcHeight
                    || lastX1 != x1 || lastY1 != y1 || lastX2 != x2 || lastY2 != y2){
                if(onScreenWindowChangedListener != null){
                    onScreenWindowChangedListener.onScreenWindowChanged(mIsFinger, srcWidth, srcHeight, x1, y1, x2, y2);
                }
                lastWidth = srcWidth;
                lastHeight = srcHeight;
                lastX1 = x1;
                lastY1 = y1;
                lastX2 = x2;
                lastY2 = y2;
            }


//            Log.d("change", "screen:" + viewWidth + "*" + viewHeight
//                    + ", picture:" + srcWidth + "*" + srcHeight
//                    + ", offset(x,y):(" + (int)mOffsetX + "," + (int)mOffsetY + ")"
//                    + ", (x,y):" + x + "," + y + ""
//                    + ", xScale:"+ xScale + ", yScale:" + yScale
//                    + ", (x1, x2, y1, y2):" + (int)x1 +","+ (int)x2 +"," + (int)y1 + "," + (int)y2);

        }

        void buildAnimal() {
            long time = System.currentTimeMillis() - mAnimaStartTime;
            if (time > mAnimaTime) {
                mScale = mStartScale + mTargeScaleOffset;
                return;
            }

            float ratio = mInterpolator.getInterpolation((float) (time * 1.0 / mAnimaTime));
            mScale = mStartScale + ratio * mTargeScaleOffset;
            requestRender();

        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//            YtxLog.d(TAG, "onSurfaceCreated");
//            GLES20.glEnable(GLES20.GL_TEXTURE_2D);
//            IntBuffer buffer = IntBuffer.allocate(1);
//            GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, buffer);
//            mMaxTextureSize = buffer.get(0);
//            GLES20.glGetError();
//            renderContext = RendererUtils.createProgram();
            if (!prog.isProgramBuilt()) {
                prog.buildProgram();
                YtxLog.d("GLFrameRenderer","GLFrameRenderer :: buildProgram done");
            }

          //  InitShaders();



        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {

        }

        @Override
        public void onDrawFrame(GL10 gl) {
            //Clear
//            if (y != null) {
//                y.position(0);
//                u.position(0);
//                v.position(0);
//            GLES20.glClearColor(0.0f,255f,0.0f,0.0f);
//            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
//
//            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
//            YtxLog.d(TAG,"_ytid="+_ytid);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _ytid);
//            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, pixel_w, pixel_h, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);
//
//            GLES20.glUniform1i(textureUniformY, 0);
//            //U
//            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _utid);
//            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, u);
//            GLES20.glUniform1i(textureUniformU, 1);
//            //V
//            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, _vtid);
//            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, pixel_w/2, pixel_h/2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, v);
//            GLES20.glUniform1i(textureUniformV, 2);
//                GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
//
       // }
//            // Draw
//
            // Show
            //Double
            //GLES20.glutSwapBuffers();
//########################################################
                       drawFrame2();
//            YtxLog.d(TAG, "ondrawframe");
//            Runnable r = null;
//            synchronized (queue) {
//                if (!queue.isEmpty()) {
//                    r = queue.remove(0);
//                }
//            }
//            if (r != null) {
//                r.run();
//            }
//            if (!queue.isEmpty()) {
//                requestRender();
//            }
//            if (mIsResume) {
//                RendererUtils.renderBackground();
//                YtxLog.d(TAG,"GraphicRenderer onDrawFrame");
//                drawFrame2();
////                if (picture != null) {
////                    buildAnimal();
////                    setRenderMatrix(picture.width(), picture.height());
////                    RendererUtils.renderTexture(renderContext, picture.texture(),
////                            viewWidth, viewHeight);
////                }
//            }
        }

        public void drawFrame2(){
//                    RendererUtils.renderTexture(renderContext, 1,
//                            600, 200);

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
                    synchronized (this) {
                        y = ByteBuffer.allocate(yarraySize);
                        u = ByteBuffer.allocate(uvarraySize);
                        v = ByteBuffer.allocate(uvarraySize);
                    }
                }
            }

          //  mParentAct.onPlayStart();
            YtxLog.d("GLFrameRenderer","INIT X");
        }

        public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){

            YtxLog.d("GraphicRenderer","yuhao renderer updateYuv ydata.length="+ydata.length+" ydata[0]="+ydata[0]);
            YtxLog.d("GraphicRenderer","yuhao renderer updateYuv udata.length="+udata.length+" udata[0]="+udata[0]);
            YtxLog.d("GraphicRenderer","yuhao renderer updateYuv vdata.length="+vdata.length+" ydata[0]="+vdata[0]);
                update(640,272);
                synchronized (this) {

                    y = ByteBuffer.allocate(ydata.length);
                    u = ByteBuffer.allocate(udata.length);
                    v = ByteBuffer.allocate(vdata.length);

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
