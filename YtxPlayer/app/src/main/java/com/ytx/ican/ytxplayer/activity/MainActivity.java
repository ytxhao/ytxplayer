package com.ytx.ican.ytxplayer.activity;


import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.ImageView;

import com.squareup.otto.Subscribe;
import com.ytx.ican.media.player.pragma.IMediaPlayer;
import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.pragma.YtxMediaPlayer;
import com.ytx.ican.media.player.view.YtxMediaController;
import com.ytx.ican.media.player.view.YtxVideoView;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.eventbus.FileExplorerEvents;
import com.ytx.ican.ytxplayer.utils.FontSearchConfig;
import com.ytx.ican.ytxplayer.utils.PreferenceUtil;
import com.ytx.ican.ytxplayer.utils.Utils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class MainActivity extends BaseActivity implements View.OnClickListener {


    private static final String TAG = "MainActivity";
    private static final String FILE_NAME = "file_name";

    private YtxVideoView ytxVideoView;
    private YtxMediaController ytxMediaController;

    private AutoCompleteTextView actvFileNameVideo;
    private AutoCompleteTextView actvFileNameSub;
    private Button btAddVideo ;
    private Button btAddSub ;
    private Button btPlay ;
    private ImageView ivDragVideo;
    private ArrayAdapter<String> adapter;
    private ArrayList<String> contacts = new ArrayList<>();
    private String [] files = new String[]{"titanic.mkv","rtmp://live.hkstv.hk.lxdns.com/live/hks"};
    private String filePath;
    private String fileName;
    private String subtitles;
    private boolean playNext = false;
    private boolean isAddVideo = false;
    private boolean isAddSub = false;
    Handler handler = new Handler(){

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case YtxMediaPlayer.MEDIA_STOPPED:
                    if(playNext){
                        playNext = false;

                        YtxLog.d(TAG,"subtitles="+subtitles);
                        if(!TextUtils.isEmpty(subtitles)){
                            ytxVideoView.setSubtitles(subtitles);
                        }
                        ytxVideoView.setVideoPath(fileName);
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
//        fileName = PreferenceUtil.getInstance().getString(FILE_NAME);

        //获取当前程序路径

        String getFilesDir=getApplicationContext().getFilesDir().getAbsolutePath();

       //获取该程序的安装包路径

        FontSearchConfig.setFontSearchPath(getFilesDir);
        YtxLog.d(TAG,"getFilesDir="+getFilesDir+" FontSearchPath="+FontSearchConfig.getFontSearchPath());

        Utils.CopyAssets(this,"video",filePath);
        Utils.CopyAssets(this,"fonts",getFilesDir);

        initView();

        ytxVideoView.setVideoPath(filePath+files[0]);
        ytxVideoView.start();

        FileExplorerEvents.getBus().register(this);

    }

    private void initView() {
        ytxVideoView = (YtxVideoView) findViewById(R.id.ytxVideoView);
        actvFileNameVideo = (AutoCompleteTextView) findViewById(R.id.actvFileNameVedio);
        actvFileNameSub = (AutoCompleteTextView) findViewById(R.id.actvFileNameSub);
        btAddVideo = (Button) findViewById(R.id.btAddVideo);
        btAddSub = (Button) findViewById(R.id.btAddSub);
        btPlay = (Button) findViewById(R.id.btPlay);
        ivDragVideo = (ImageView) findViewById(R.id.ivDragVideo);

        ivDragVideo.setOnClickListener(this);
        btAddVideo.setOnClickListener(this);
        btAddSub.setOnClickListener(this);

        btPlay.setOnClickListener(this);
        btPlay.setEnabled(false);
        actvFileNameVideo.addTextChangedListener(textWatcher);
        actvFileNameVideo.setHorizontallyScrolling(true);
        actvFileNameVideo.setOnClickListener(this);

        actvFileNameSub.addTextChangedListener(textWatcher);
        actvFileNameSub.setHorizontallyScrolling(true);
        actvFileNameSub.setOnClickListener(this);

        adapter = new ArrayAdapter<>(this,R.layout.file_name_dropdow_item,contacts);
        actvFileNameVideo.setAdapter(adapter);


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

        ytxVideoView.setOnErrorListener(new IMediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(IMediaPlayer mp, int what, int extra) {
                YtxLog.d(TAG,"setOnErrorListener onError what="+what);
                switch (what){
                    case YtxMediaPlayer.MEDIA_ERROR_OPEN_STREAM:
                        getHelper().showMessage(R.string.file_open_failed);
                        break;
                    case YtxMediaPlayer.MEDIA_ERROR_OPEN_STREAM_IS_SUBTITLES:
                        getHelper().showMessage(R.string.file_open_is_subtitle);
                        break;

                }

                ytxVideoView.onDestroy();
                return true;
            }
        });

        ytxVideoView.setOnPreparedListener(new IMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(IMediaPlayer mp) {
                YtxLog.d(TAG,"setOnPreparedListener onPrepared");
            }
        });

        ytxVideoView.setOnCompletionListener(new IMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(IMediaPlayer mp) {
                YtxLog.d(TAG,"setOnCompletionListener onCompletion");
            }
        });

       // showLoading();
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
        YtxLog.d(TAG,"#### #### onPause");
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
        FileExplorerEvents.getBus().unregister(this);
        ytxVideoView.onDestroy();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        YtxLog.d(TAG,"#### #### onConfigurationChanged");
    }



    private TextWatcher textWatcher = new TextWatcher() {

        @Override
        public void afterTextChanged(Editable s) {
            if (TextUtils.isEmpty(actvFileNameVideo.getText().toString().trim()))
                btPlay.setEnabled(false);
            else
                btPlay.setEnabled(true);

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
        Intent intent;
        int id = v.getId();

        switch (id){
            case R.id.btAddVideo:
                isAddVideo = true;
                intent = new Intent(this, FileExplorerActivity.class);
                startActivity(intent);
                break;
            case R.id.btAddSub:
                isAddSub = true;
                intent = new Intent(this, FileExplorerActivity.class);
                startActivity(intent);
                break;
            case R.id.ivDragVideo:
                actvFileNameVideo.showDropDown();
                break;
            case R.id.btPlay:
                PreferenceUtil.getInstance().putString(FILE_NAME,actvFileNameVideo.getText().toString().trim());
                fileName = actvFileNameVideo.getText().toString().trim();
                subtitles = actvFileNameSub.getText().toString().trim();
                playNext = true;

                if(ytxVideoView.getYtxVideoViewCurrentState() == YtxVideoView.STATE_IDLE){
                    if(!TextUtils.isEmpty(subtitles)){
                        ytxVideoView.setSubtitles(subtitles);
                    }
                    ytxVideoView.setVideoPath(fileName);
                    ytxVideoView.start();
                }else{
                    ytxVideoView.onDestroy();
                }
                break;
        }
    }


    @Subscribe
    public void onClickFile(FileExplorerEvents.OnClickFile event) {
        File f = event.mFile;
        try {
            f = f.getAbsoluteFile();
            f = f.getCanonicalFile();
            if (TextUtils.isEmpty(f.toString()))
                f = new File("/");
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (f.isDirectory()) {


        } else if (f.exists()) {

            YtxLog.d(TAG,"f.getPath()="+f.getPath()+" f.getName()="+f.getName());

            if(isAddSub){
                actvFileNameSub.setText(f.getPath());
                isAddSub = false;
            }else if(isAddVideo){
                actvFileNameVideo.setText(f.getPath());
                isAddVideo = false;
            }



        }
    }
}