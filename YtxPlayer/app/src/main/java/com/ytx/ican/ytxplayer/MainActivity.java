package com.ytx.ican.ytxplayer;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.ImageView;

import com.ytx.ican.media.player.pragma.IMediaPlayer;
import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.pragma.YtxMediaPlayer;
import com.ytx.ican.media.player.view.YtxMediaController;
import com.ytx.ican.media.player.view.YtxVideoView;
import com.ytx.ican.ytxplayer.utils.PreferenceUtil;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private static final String TAG = "MainActivity2";
    private static final String FILE_NAME = "file_name";

    private YtxVideoView ytxVideoView;
    private YtxMediaController ytxMediaController;

    private AutoCompleteTextView actvFileName;
    private Button bt ;
    private ImageView ivDrag;
    private ArrayAdapter<String> adapter;
    private ArrayList<String> contacts = new ArrayList<>();
    private String [] files = new String[]{"test_file/x7_11.mkv","video2.mp4","titanic.mkv","xszr.mp4","xszr2.mkv","out.avi","rtmp://live.hkstv.hk.lxdns.com/live/hks"};
    private String filePath;
    private String fileName;
    private boolean playNext = false;
    Handler handler = new Handler(){

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case YtxMediaPlayer.MEDIA_STOPPED:
                    if(playNext){
                        playNext = false;
                        if(!TextUtils.isEmpty(fileName)){
//                            if(!fileName.equals(files[4])){
//                                ytxVideoView.setVideoPath(filePath+fileName);
//                            }else{
//                                ytxVideoView.setVideoPath(fileName);
//                            }
                            if(fileName.contains("rtmp://")){
                                ytxVideoView.setVideoPath(fileName);
                            }else{
                                ytxVideoView.setVideoPath(filePath+fileName);
                            }
                        }else{
                            ytxVideoView.setVideoPath(filePath+files[0]);
                        }
                        ytxVideoView.start();
                    }

                    break;
                default:
                    return;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        YtxLog.d(TAG,"#### #### onCreate");
        setContentView(R.layout.activity_main);
        PreferenceUtil.initInstance(getApplicationContext(), PreferenceUtil.MODE_ENCRYPT_ALL);

        filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;
        fileName = PreferenceUtil.getInstance().getString(FILE_NAME);

        CopyAssets(this,"video",filePath);
        CopyAssets(this,"fonts",filePath);
        CopyAssets(this,"ass",filePath);

        initView();

       // playVideo();

        if(!TextUtils.isEmpty(fileName)){
            if(!fileName.equals(files[4])){
                ytxVideoView.setVideoPath(filePath+fileName);
            }else{
                ytxVideoView.setVideoPath(fileName);
            }
        }else{
            ytxVideoView.setVideoPath(filePath+files[0]);
        }

        ytxVideoView.start();



    }

    private void initView() {
        ytxVideoView = (YtxVideoView) findViewById(R.id.ytxVideoView);
        actvFileName = (AutoCompleteTextView) findViewById(R.id.actvFileName);
        bt = (Button) findViewById(R.id.bt);
        ivDrag = (ImageView) findViewById(R.id.ivDrag);

        ivDrag.setOnClickListener(this);
        bt.setOnClickListener(this);
        bt.setEnabled(false);
        actvFileName.addTextChangedListener(textWatcher);
        actvFileName.setHorizontallyScrolling(true);
        actvFileName.setOnClickListener(this);
        actvFileName.setText(fileName);
        for(int i=0;i<files.length;i++){
            contacts.add(files[i]);
        }

        adapter = new ArrayAdapter<>(this,R.layout.file_name_dropdow_item,contacts);
        actvFileName.setAdapter(adapter);


        ytxMediaController = new YtxMediaController(this);
        ytxVideoView.setMediaController(ytxMediaController);
        ytxVideoView.requestFocus();

        ytxVideoView.setOnInfoListener(new IMediaPlayer.OnInfoListener() {
            @Override
            public boolean onInfo(IMediaPlayer mp, int what, int extra) {
                handler.sendEmptyMessage(what);
                return false;
            }
        });
    }


    @Override
    protected void onResume() {
        super.onResume();
        YtxLog.d(TAG,"#### #### onResume");
        ytxVideoView.onResume();

    }


    @Override
    protected void onPause() {
        super.onPause();
        YtxLog.d("MainActivity2","#### #### onPause");
        ytxVideoView.onPause();

    }

    @Override
    protected void onStart() {
        super.onStart();
        YtxLog.d(TAG,"#### #### onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();
        YtxLog.d(TAG,"#### #### onStop");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        YtxLog.d(TAG,"#### #### onDestroy");
        ytxVideoView.onDestroy();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        YtxLog.d(TAG,"#### #### onConfigurationChanged");
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

    @Override
    public void onClick(View v) {

        int id = v.getId();

        switch (id){
            case R.id.bt:
                PreferenceUtil.getInstance().putString(FILE_NAME,actvFileName.getText().toString().trim());
                fileName = actvFileName.getText().toString().trim();
                playNext = true;
                ytxVideoView.onDestroy();

                break;
            case R.id.ivDrag:
                actvFileName.showDropDown();
                break;
        }
    }


}
