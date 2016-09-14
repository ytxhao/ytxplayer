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

import java.nio.IntBuffer;
import java.util.Vector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2016/9/10.
 */

public class GraphicGLSurfaceView extends GLSurfaceView {


    public static final String TAG = "GraphicGLSurfaceView";
    private  GraphicRenderer renderer;
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
        super(context);
    }

    public GraphicGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        if(!supportsOpenGLES2(context)){
            throw new RuntimeException("not support gles 2.0");
        }
        renderer = new GraphicRenderer();
        setEGLContextClientVersion(2);
        setEGLConfigChooser(new CustomChooseConfig2.ComponentSizeChooser(8, 8, 8, 8, 0, 0));
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
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
        if (!isInitial) {
            isInitial = true;
            initial();
        }
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

        RendererUtils.RenderContext renderContext;
        Picture picture;

        int viewWidth;
        int viewHeight;
        int lastWidth, lastHeight, lastX1, lastY1, lastX2, lastY2;

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
            YtxLog.d(TAG, "onSurfaceCreated");
            GLES20.glEnable(GLES20.GL_TEXTURE_2D);
            IntBuffer buffer = IntBuffer.allocate(1);
            GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, buffer);
            mMaxTextureSize = buffer.get(0);
            GLES20.glGetError();
            renderContext = RendererUtils.createProgram();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {

        }

        @Override
        public void onDrawFrame(GL10 gl) {
            YtxLog.d(TAG, "ondrawframe");
            Runnable r = null;
            synchronized (queue) {
                if (!queue.isEmpty()) {
                    r = queue.remove(0);
                }
            }
            if (r != null) {
                r.run();
            }
            if (!queue.isEmpty()) {
                requestRender();
            }
            if (mIsResume) {
                RendererUtils.renderBackground();
                drawFrame();
                if (picture != null) {
                    buildAnimal();
                    setRenderMatrix(picture.width(), picture.height());
                    RendererUtils.renderTexture(renderContext, picture.texture(),
                            viewWidth, viewHeight);
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


    }
}
