package com.ytx.ican.ytxplayer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    }

    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
//        System.loadLibrary("native-lib");
//
//        System.loadLibrary("avutil");
//        System.loadLibrary("swscale");
//        System.loadLibrary("swresample");
//        System.loadLibrary("avcodec");
//        System.loadLibrary("avformat");
//        System.loadLibrary("avfilter");

        System.loadLibrary("gnustl_shared");
        System.loadLibrary("mp4v2");
        System.loadLibrary("faad");
        System.loadLibrary("faac");
        System.loadLibrary("rtmp");
        System.loadLibrary("x264");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("postproc");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avdevice");
        System.loadLibrary("avfilter");

        System.loadLibrary("native-lib");


    }
}
