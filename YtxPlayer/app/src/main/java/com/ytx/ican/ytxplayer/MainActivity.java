package com.ytx.ican.ytxplayer;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.MediaController;
import android.widget.VideoView;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.view.YtxMediaController;
import com.ytx.ican.media.player.view.YtxVideoView;


import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("native-lib");
    }

    YtxVideoView ytxVideoView;
    YtxMediaController ytxMediaController;
    VideoView mVideoView;
    MediaController mMediaController;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        YtxLog.d("MainActivity","#### #### onCreate");
        setContentView(R.layout.activity_main);
        String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;
        // .getAbsolutePath() + "/" ;
        YtxLog.d("MainActivity","filePath="+filePath);
        CopyAssets(this,"video",filePath);

        ytxMediaController = new YtxMediaController(this);
        ytxVideoView = (YtxVideoView) findViewById(R.id.ytxVideoView);


        ytxVideoView.setMediaController(ytxMediaController);


        ytxVideoView.requestFocus();
       // videoView.setMediaController();
       // videoView.setVideoPath(filePath+"titanic.mkv");
        ytxVideoView.setVideoPath(filePath+"video2.mp4");

      //  videoView.setVideoPath("rtmp://live.hkstv.hk.lxdns.com/live/hks"); //mPlayer.setDataSource("rtmp://live.hkstv.hk.lxdns.com/live/hks");

        ytxVideoView.start();

        //########################################

        mVideoView = (VideoView) findViewById(R.id.mVideoView);
        mMediaController = new MediaController(this);
        mVideoView.setVideoPath(filePath+"titanic.mkv");

        mVideoView.setMediaController(mMediaController);


        //让VideiView获取焦点
        mVideoView.requestFocus();





    }


    @Override
    protected void onResume() {
        super.onResume();
        YtxLog.d("MainActivity","#### #### onResume");

        //mGLSurface.onResume();
    }


    @Override
    protected void onPause() {
        super.onPause();
        YtxLog.d("MainActivity","#### #### onPause");
    }

    @Override
    protected void onStart() {
        super.onStart();
        YtxLog.d("MainActivity","#### #### onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();
        YtxLog.d("MainActivity","#### #### onStop");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        YtxLog.d("MainActivity","#### #### onDestroy");
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        YtxLog.d("MainActivity","#### #### onConfigurationChanged");
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
