
package com.ytx.ican.media.player.render;

import android.content.Context;
import android.opengl.GLES20;
import android.os.Environment;
import android.util.AttributeSet;

import com.ytx.ican.media.player.YtxLog;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;


public class VideoGlSurfaceViewFFMPEG extends VideoGlSurfaceView {

    private final static String TAG = "VideoDecoderFFMPEG";

    YUVFilter mYUVFilter;
    Picture mPhoto;
    int mWidth=2;
    int mHeight;
    int mYUVTextures[] = new int[3];
    volatile boolean mInitialed = false;
  //  H264Decoder mH264Decoder;

    FileOutputStream fos = null;
    public VideoGlSurfaceViewFFMPEG(Context context){
       // super(context);
//        this(context,null,null);
        this(context,null);

    }

    public VideoGlSurfaceViewFFMPEG(Context context, AttributeSet attrs) {
        super(context, attrs);
        initData();
    }

//    public VideoGlSurfaceViewFFMPEG(Context context, AttributeSet attrs, HardDecodeExceptionCallback callback) {
//        super(context, attrs, callback);
//    }


    public void initData(){
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/recordmicinput.yuv");
        // Delete any previous recording.
        if (file.exists())
            file.delete();

        try {
            fos = new FileOutputStream(file);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void initial() {
        super.initial();
        YtxLog.d(TAG,"initial yuhaoo");
      //  mH264Decoder = new H264Decoder();
        mYUVFilter = new YUVFilter(getContext());
        YtxLog.d(TAG,"initial yuhaoo 1");
        mYUVFilter.initial();
        YtxLog.d(TAG,"initial yuhaoo 2");
        GLES20.glGenTextures(mYUVTextures.length, mYUVTextures, 0);
        YtxLog.d(TAG,"initial mYUVTextures[0]="+mYUVTextures[0]);
        YtxLog.d(TAG,"initial mYUVTextures[1]="+mYUVTextures[1]);
        YtxLog.d(TAG,"initial mYUVTextures[2]="+mYUVTextures[2]);
        mYUVFilter.setYuvTextures(mYUVTextures);
        mInitialed = true;
        YtxLog.d(TAG,"initial OUT");
    }

    @Override
    protected void release() {
        super.release();
        mInitialed = false;
//        if(mH264Decoder!=null) {
//            mH264Decoder.release();
//            mH264Decoder = null;
//        }
        if (mYUVFilter != null) {
            mYUVFilter.release();
            mYUVFilter = null;
            GLES20.glDeleteTextures(mYUVTextures.length, mYUVTextures, 0);
        }
        if (mPhoto != null) {
            mPhoto.clear();
            mPhoto = null;
        }
    }

    @Override
    public void drawFrame() {
        super.drawFrame();
        if (!mInitialed) {
            return;
        }
        VideoFrame frame = mAVFrameQueue.poll();
        if (frame == null || frame.data == null) {
            return;
        }
        long lastTime = System.currentTimeMillis();
        if (frame.width != mWidth || frame.height != mHeight) {
            mWidth = frame.width;
            mHeight = frame.height;
//            if(mH264Decoder!=null) {
//                mH264Decoder.release();
//            }
//            mH264Decoder = new H264Decoder();
        }

        // ByteBuffer buffer = ByteBuffer.wrap(frame.data);
        // buffer.flip();
        // if (H264Decoder.decodeBuffer(buffer, buffer.limit(),
        // frame.timeStamp))// (frame.data,frame.data.length,frame.timeStamp))
//        if (mH264Decoder.decode(frame.data, frame.data.length, frame.timeStamp))
//        {
//            int ret = mH264Decoder.toTexture(mYUVTextures[0], mYUVTextures[1], mYUVTextures[2]);
//            if (ret < 0)
//                return;
//
//            if (mPhoto == null) {
//                mPhoto = Picture.create(mH264Decoder.getWidth(), mH264Decoder.getHeight());
//            } else {
//                mPhoto.updateSize(mH264Decoder.getWidth(), mH264Decoder.getHeight());
//            }
//
//            mYUVFilter.process(null, mPhoto);
//            Picture dst = appFilter(mPhoto);
//            RendererUtils.checkGlError("process");
//            setPicture(dst);
//            RendererUtils.checkGlError("setPhoto");
//        }

        if (mAVFrameQueue.size() > 0) {
            requestRender();
        }

        long decodeOneFrameMilliseconds = System.currentTimeMillis() - lastTime;
        onDecodeTime(decodeOneFrameMilliseconds);

        YtxLog.d(TAG, "decode " + frame.toString() + ", decodeTime:" + decodeOneFrameMilliseconds);

    }

    public void getTextureData(){

    }

    public void updateYuv(byte[] ydata, byte[] udata, byte[] vdata){
        super.updateYuv(ydata,udata,vdata);
        try {
            fos.write(ydata,0,ydata.length);
            fos.write(udata,0,udata.length);
            fos.write(vdata,0,vdata.length);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onResume() {
        YtxLog.d(TAG,"onResume yuhaoo");
        super.onResume();

    }

}
