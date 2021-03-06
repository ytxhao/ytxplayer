package com.ytx.ican.ytxplayer.activity;


import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.utils.Utils;

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
        System.loadLibrary("native-ffmpeg-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main1);
        final String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/";

        Utils.CopyAssets(this,"video",filePath);
        Utils.CopyAssets(this,"fonts",filePath);

        new Thread(new Runnable() {
            @Override
            public void run() {

                setCmd("ffmpeg -i /storage/emulated/0/titanic.mkv -vf movie=/storage/emulated/0/my_logo.png,scale=100:100[watermask];[in][watermask]overlay=10:10[out] -y /storage/emulated/0/out.mp4");
            //    setCmd("ffmpeg -i /storage/emulated/0/test_file/x7_11.mkv -vf subtitles=/storage/emulated/0/test_file/x7_11.srt /storage/emulated/0/test_file/out.avi");
            }
        }).start();

    }

    @Override
    public void onClick(View v) {

    }

    native int setCmd(String cmd);

}
