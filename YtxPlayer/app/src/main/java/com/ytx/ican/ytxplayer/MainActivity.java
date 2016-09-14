package com.ytx.ican.ytxplayer;

import android.content.Context;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.ytx.ican.media.player.YtxLog;
import com.ytx.ican.media.player.YtxMediaPlayer;
import com.ytx.ican.media.player.render.VideoGlSurfaceViewFFMPEG;
import com.ytx.ican.media.player.test.GLFrameRenderer;
import com.ytx.ican.media.player.test.GLFrameSurface;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {


    VideoGlSurfaceViewFFMPEG mGLSurface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
       // tv.setText(stringFromJNI());
        YtxMediaPlayer mPlayer = new YtxMediaPlayer();
        String filePath = android.os.Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;
        YtxLog.d("MainActivity","filePath="+filePath);
        CopyAssets(this,"video",filePath);
        //----------------------------------------
        mGLSurface =  new  VideoGlSurfaceViewFFMPEG(this);//(VideoGlSurfaceViewFFMPEG) findViewById(R.id.glsurface);
        mPlayer.setSurfaceView(mGLSurface);
        //----------------------------------------
        try {
            mPlayer.setDataSource(filePath+"titanic.mkv");
        } catch (IOException e) {
            e.printStackTrace();
        }
        mPlayer.prepare();
        mPlayer.start();


    }

    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
/*
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
        */


    }

    /**
     * 复制asset文件到指定目录
     * @param oldPath  asset下的路径
     * @param newPath  SD卡下保存路径
     */
    public static void CopyAssets(Context context, String oldPath, String newPath) {
        try {
            String fileNames[] = context.getAssets().list(oldPath);// 获取assets目录下的所有文件及目录名
            if (fileNames.length > 0) {// 如果是目录
                File file = new File(newPath);
                file.mkdirs();// 如果文件夹不存在，则递归
                for (String fileName : fileNames) {
                    CopyAssets(context, oldPath + "/" + fileName, newPath + "/" + fileName);
                }
            } else {// 如果是文件
                InputStream is = context.getAssets().open(oldPath);
                FileOutputStream fos = new FileOutputStream(new File(newPath));
                byte[] buffer = new byte[1024];
                int byteCount = 0;
                while ((byteCount = is.read(buffer)) != -1) {// 循环从输入流读取
                    // buffer字节
                    fos.write(buffer, 0, byteCount);// 将读取的输入流写入到输出流
                }
                fos.flush();// 刷新缓冲区
                is.close();
                fos.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
