package com.ytx.ican.ytxplayer;


import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.ytx.ican.media.player.pragma.YtxLog;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

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
        final String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/";

        CopyAssets(this,"video",filePath);
        CopyAssets(this,"fonts",filePath);
        CopyAssets(this,"ass",filePath);

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
