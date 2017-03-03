package com.ytx.ican.ytxplayer;


import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.ytx.ican.media.player.pragma.YtxLog;

public class MainActivity2 extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("mp4v2");
        System.loadLibrary("faad");
        System.loadLibrary("faac");
        System.loadLibrary("rtmp");
        System.loadLibrary("x264");

        System.loadLibrary("charset");
        System.loadLibrary("iconv");
        System.loadLibrary("xml2");

        System.loadLibrary("png");
        System.loadLibrary("harfbuzz");
        System.loadLibrary("freetype");

        System.loadLibrary("fontconfig");
        System.loadLibrary("fribidi");
        System.loadLibrary("ass");


        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("postproc");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avdevice");
        System.loadLibrary("avfilter");
        System.loadLibrary("native-ffplay-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main1);
        String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/";
        setCmd("ffplay -vf subtitles="+filePath+"test_file/x7_11.srt "+filePath+"test_file/x7_11.mkv");
    }

    @Override
    public void onClick(View v) {

    }

    native int setCmd(String cmd);
}
