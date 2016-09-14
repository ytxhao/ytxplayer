package com.ytx.ican.media.player.render;

/**
 * Created by Administrator on 2016/9/12.
 */

public interface HardDecodeExceptionCallback {

    public void onHardDecodeException(Exception e);

    public void onOtherException(Throwable e);

    public void onDecodePerformance(long decodeOneFrameMilliseconds);

}
