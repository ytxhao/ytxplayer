package com.ytx.ican.ytxplayer;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Environment;
import android.support.annotation.NonNull;
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

       // isGrantExternalRW(this);
       // tv.setText(stringFromJNI());

        YtxMediaPlayer mPlayer = new YtxMediaPlayer();
     //   android.os.Environment.get
     //   android.os.Environment.get
        String filePath = android.os.Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;
               // .getAbsolutePath() + "/" ;
        YtxLog.d("MainActivity","filePath="+filePath);
        CopyAssets(this,"video",filePath);
        //----------------------------------------
      //  mGLSurface =  new  VideoGlSurfaceViewFFMPEG(this);//(VideoGlSurfaceViewFFMPEG) findViewById(R.id.glsurface);
        mGLSurface = (VideoGlSurfaceViewFFMPEG) findViewById(R.id.surface);
        mPlayer.setSurfaceView(mGLSurface);
        //----------------------------------------
        try {
            mPlayer.setDataSource(filePath+"titanic.mkv");
            //mPlayer.setDataSource(filePath+"video2.mp4");
        } catch (IOException e) {
            e.printStackTrace();
        }
        mPlayer.prepare();
        mPlayer.start();


    }


    @Override
    protected void onResume() {
        super.onResume();
        //mGLSurface.onResume();
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

    public static boolean isGrantExternalRW(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && activity.checkSelfPermission(
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {

            activity.requestPermissions(new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
            }, 1);

            return false;
        }

        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1) {
            for (int i = 0; i < permissions.length; i++) {
                String permission = permissions[i];
                int grantResult = grantResults[i];

                if (permission.equals(Manifest.permission.READ_EXTERNAL_STORAGE)) {
                    if (grantResult == PackageManager.PERMISSION_GRANTED) {
                        //授权成功后的逻辑
                      //  ...
                    } else {
                      //  requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, PERMISSIONS_CODE);
                    }
                }
            }
        }
    }
}
