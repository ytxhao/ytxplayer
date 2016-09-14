package com.ytx.ican.media.player.render;

import android.content.Context;
import android.util.AttributeSet;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Created by Administrator on 2016/9/12.
 */

public class VideoGlSurfaceView extends GraphicGLSurfaceView {
    protected LinkedBlockingQueue<VideoFrame> mAVFrameQueue = new LinkedBlockingQueue<VideoFrame>(30);
    protected WeakReference<HardDecodeExceptionCallback> mHardDecodeExceptionCallback;

    Filter mFilter;
    Picture mMiddlePhoto;

    public VideoGlSurfaceView(Context context, AttributeSet attrs, HardDecodeExceptionCallback callback) {
        super(context, attrs);
        if(callback!=null)
            this.mHardDecodeExceptionCallback = new WeakReference<HardDecodeExceptionCallback>(callback);
    }

    public void drawVideoFrame(VideoFrame frame) {
        try {
            mAVFrameQueue.put(frame);
            requestRender();
        } catch (InterruptedException e) {
        }
    }

    public void setFilter(final Filter filter){
        queue(new Runnable() {
            @Override
            public void run() {
                if(mFilter!=null){
                    mFilter.release();
                }
                mFilter = filter;
                mFilter.initial();
            }
        });
    }

    protected Picture appFilter(Picture src){
        if(mFilter!=null){
            if(mMiddlePhoto==null && src!=null){
                mMiddlePhoto = Picture.create(src.width(),src.height());
            }
            mFilter.process(src,mMiddlePhoto);
            return mMiddlePhoto;
        }
        return src;
    }

    @Override
    protected void initial() {
        super.initial();
        if(mFilter!=null){
            mFilter.initial();
        }
        mAVFrameQueue.clear();
    }

    @Override
    protected void release() {
        super.release();
        if(mFilter!=null){
            mFilter.release();
        }
        if(mMiddlePhoto!=null){
            mMiddlePhoto.clear();
            mMiddlePhoto = null;
        }
        mAVFrameQueue.clear();
    }

    public void onHardDecodeException(Exception e){
        if(mHardDecodeExceptionCallback!=null && mHardDecodeExceptionCallback.get()!=null){
            mHardDecodeExceptionCallback.get().onHardDecodeException(e);
        }
    }

    public void onOtherException(Throwable e){
        if(mHardDecodeExceptionCallback!=null && mHardDecodeExceptionCallback.get()!=null){
            mHardDecodeExceptionCallback.get().onOtherException(e);
        }
    }


    final int MAX_COUNT_TIME = 20;
    final int MAX_TIME_SIZE = 5;
    int frameCount = 0;
    List<Long> decodeTimes = new ArrayList<>();

    public void onDecodeTime(long decodeOneFrameMilliseconds){

        if(decodeTimes.size() >= MAX_TIME_SIZE){
            decodeTimes.remove(0);
        }
        decodeTimes.add(decodeOneFrameMilliseconds);

        if(decodeTimes.size() > 0 && frameCount >= MAX_COUNT_TIME){
            long averageDecodeTime = 0;
            for(long mTime : decodeTimes){
                averageDecodeTime += mTime;
            }
            averageDecodeTime = averageDecodeTime/decodeTimes.size();
            if(mHardDecodeExceptionCallback!=null && mHardDecodeExceptionCallback.get()!=null){
                mHardDecodeExceptionCallback.get().onDecodePerformance(averageDecodeTime);
            }
            frameCount = 0;
        }else{
            frameCount++;
        }

    }
}
