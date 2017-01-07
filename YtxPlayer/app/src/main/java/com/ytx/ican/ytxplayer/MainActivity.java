package com.ytx.ican.ytxplayer;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.MediaController;
import android.widget.VideoView;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.view.YtxMediaController;
import com.ytx.ican.media.player.view.YtxVideoView;


import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    static {
        System.loadLibrary("native-lib");
    }

    YtxVideoView ytxVideoView;
    YtxMediaController ytxMediaController;
    VideoView mVideoView;
    MediaController mMediaController;
    AutoCompleteTextView actvFileName;
    Button bt ;
    ImageView ivDrag;
    ArrayAdapter<String> adapter;
    ArrayList<String> contacts = new ArrayList<>();
    String [] files = new String[]{"video2.mp4","titanic.mkv","xszr.mp4","rtmp://live.hkstv.hk.lxdns.com/live/hks"};
    String filePath;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        YtxLog.d("MainActivity","#### #### onCreate");
        setContentView(R.layout.activity_main);

        initView();

        filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;

        CopyAssets(this,"video",filePath);

        mMediaController = new MediaController(this);
    //    mVideoView.setVideoPath(filePath+"xszr.mp4");
    //    mVideoView.setVideoPath(filePath+"titanic.mkv");
        mVideoView.setVideoPath(filePath+"video2.mp4");
        mVideoView.setMediaController(mMediaController);
        mVideoView.requestFocus();
    }

    private void initView() {
        ytxVideoView = (YtxVideoView) findViewById(R.id.ytxVideoView);
        mVideoView = (VideoView) findViewById(R.id.mVideoView);
        actvFileName = (AutoCompleteTextView) findViewById(R.id.actvFileName);
        bt = (Button) findViewById(R.id.bt);
        ivDrag = (ImageView) findViewById(R.id.ivDrag);

        ivDrag.setOnClickListener(this);
        bt.setOnClickListener(this);
        bt.setEnabled(false);
        actvFileName.addTextChangedListener(textWatcher);
        actvFileName.setHorizontallyScrolling(true);
        actvFileName.setOnClickListener(this);

        for(int i=0;i<files.length;i++){
            contacts.add(files[i]);
        }

        adapter = new ArrayAdapter<>(this,R.layout.file_name_dropdow_item,contacts);
        actvFileName.setAdapter(adapter);


        ytxMediaController = new YtxMediaController(this);
        ytxVideoView.setMediaController(ytxMediaController);
        ytxVideoView.requestFocus();
    }


    @Override
    protected void onResume() {
        super.onResume();
        YtxLog.d("MainActivity","#### #### onResume");
        ytxVideoView.onResume();

    }


    @Override
    protected void onPause() {
        super.onPause();
        YtxLog.d("MainActivity","#### #### onPause");
        ytxVideoView.onPause();
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
        ytxVideoView.onDestroy();
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

    private TextWatcher textWatcher = new TextWatcher() {

        @Override
        public void afterTextChanged(Editable s) {
            if (TextUtils.isEmpty(actvFileName.getText().toString().trim()))
                bt.setEnabled(false);
            else
                bt.setEnabled(true);

        }

        @Override
        public void beforeTextChanged(CharSequence s, int start, int count,
                                      int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before,
                                  int count) {

        }
    };


    private void playVideo() {

        ytxVideoView.setVideoPath(filePath+actvFileName.getText().toString().trim());
        ytxVideoView.start();
    }

    @Override
    public void onClick(View v) {

        int id = v.getId();

        switch (id){
            case R.id.bt:
                playVideo();
                break;
            case R.id.ivDrag:
                actvFileName.showDropDown();
                break;
        }
    }


}
