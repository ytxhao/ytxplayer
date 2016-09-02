package com.ytx.ican.media.player;

import java.io.IOException;

/**
 * Created by Administrator on 2016/9/2.
 */

public interface IMediaPlayer {

    void setDataSource(String path)
            throws IOException, IllegalArgumentException, SecurityException, IllegalStateException;

    String getDataSource();

    void prepareAsync() throws IllegalStateException;

    void prepare() throws IllegalStateException;

    void start() throws IllegalStateException;

    void stop() throws IllegalStateException;

    void pause() throws IllegalStateException;

    void setScreenOnWhilePlaying(boolean screenOn);

    int getVideoWidth();

    int getVideoHeight();

    boolean isPlaying();

    void seekTo(long msec) throws IllegalStateException;

    long getCurrentPosition();

    long getDuration();

    void release();

    void reset();

    void setVolume(float leftVolume, float rightVolume);

    int getAudioSessionId();

}
