package com.ytx.ican.ytxplayer.activity;


import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.Filter;
import android.widget.Filterable;
import android.widget.ImageView;
import android.widget.TextView;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.media.player.view.YtxMediaController;
import com.ytx.ican.media.player.view.YtxVideoView;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.constants.ActivityResultConst;
import com.ytx.ican.ytxplayer.constants.KeyConst;
import com.ytx.ican.ytxplayer.utils.FontSearchConfig;
import com.ytx.ican.ytxplayer.utils.PreferenceUtil;
import com.ytx.ican.ytxplayer.utils.Utils;
import java.util.ArrayList;

public class VideoMainActivity extends SimpleBarRootActivity implements View.OnClickListener {


    private static final String TAG = "VideoMainActivity";
    private static final String FILE_NAME = "file_name";

    private YtxVideoView ytxVideoView;
    private YtxMediaController ytxMediaController;

    private AutoCompleteTextView actvFileNameVideo;
    private AutoCompleteTextView actvFileNameSub;

    private Button btAddSub ;
    private Button btPlay ;
    private Button btFullScreen;
    private ImageView ivDragVideo;
    private AutoCompleteAdapter mAdapter;
    private ArrayList<String> contacts = new ArrayList<>();
    private String [] files = new String[]{
            "rtmp://47.90.99.189:1935/live/demo",
            "rtmp://live.hkstv.hk.lxdns.com/live/hks",
            "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov"};
    private String filePath;
    private String fileName;
    private String subtitles;
    private String mVideoPath;
    private boolean playNext = false;
    private boolean isAddVideo = false;
    private boolean isAddSub = false;
    private boolean isFullScreen = false;
    private int mVideoCurrentTime;

    public static Intent newIntent(Context context, String videoPath, String videoTitle) {
        Intent intent = new Intent(context, VideoMainActivity.class);
        intent.putExtra("videoPath", videoPath);
        intent.putExtra("videoTitle", videoTitle);
        return intent;
    }

    public static void intentTo(Context context, String videoPath, String videoTitle) {
        context.startActivity(newIntent(context, videoPath, videoTitle));
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        YtxLog.d(TAG,"#### #### onCreate");
        setContentView(R.layout.activity_main);
        addMenu(R.id.videoSettings, R.drawable.ic_setting);
        hideBaseLine(true);
        PreferenceUtil.initInstance(getApplicationContext(), PreferenceUtil.MODE_ENCRYPT_ALL);

        filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;

        //获取当前程序路径

        String getFilesDir=getApplicationContext().getFilesDir().getAbsolutePath();

       //获取该程序的安装包路径

//        FontSearchConfig.setFontSearchPath(getFilesDir);
       // YtxLog.d(TAG,"getFilesDir="+getFilesDir+" FontSearchPath="+FontSearchConfig.getFontSearchPath());

//        Utils.CopyAssets(this,"video",filePath);
//        Utils.CopyAssets(this,"fonts",getFilesDir);

        initView();


      //  ytxVideoView.setVideoPath(filePath+files[0]);
     //   ytxVideoView.start();

     //   FileExplorerEvents.getBus().register(this);

    }

    private void initView() {
        ytxVideoView = (YtxVideoView) findViewById(R.id.ytxVideoView);
        actvFileNameVideo = (AutoCompleteTextView) findViewById(R.id.actvFileNameVedio);
        actvFileNameSub = (AutoCompleteTextView) findViewById(R.id.actvFileNameSub);

        btAddSub = (Button) findViewById(R.id.btAddSub);
        btPlay = (Button) findViewById(R.id.btPlay);
        btFullScreen = (Button) findViewById(R.id.btFullScreen);
        ivDragVideo = (ImageView) findViewById(R.id.ivDragVideo);

        ivDragVideo.setOnClickListener(this);

        btAddSub.setOnClickListener(this);
        btFullScreen.setOnClickListener(this);
        btPlay.setOnClickListener(this);
        btPlay.setEnabled(false);
        actvFileNameVideo.addTextChangedListener(textWatcher);
        actvFileNameVideo.setHorizontallyScrolling(true);
        actvFileNameVideo.setOnClickListener(this);

        actvFileNameSub.addTextChangedListener(textWatcher);
        actvFileNameSub.setHorizontallyScrolling(true);
        actvFileNameSub.setOnClickListener(this);

        mVideoPath = getIntent().getStringExtra("videoPath");
        contacts.add(mVideoPath);

        for(int i=0;i<files.length;i++){
            contacts.add(files[i]);
        }

        actvFileNameVideo.setText(contacts.get(0));
        mAdapter = new AutoCompleteAdapter(this,contacts);

        actvFileNameVideo.setAdapter(mAdapter);

//        actvFileNameVideo.setText("rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov");
        ytxMediaController = new YtxMediaController(this);
        ytxVideoView.setMediaController(ytxMediaController);
        ytxVideoView.requestFocus();

    }

    @Override
    public void onMenuItemClick(View item) {
        super.onMenuItemClick(item);
        Intent intent = new Intent(this,SettingActivity.class);
        startActivity(intent);

    }

    @Override
    protected void onResume() {
        super.onResume();
        YtxLog.d(TAG,"#### #### onResume");


//        if() {
//            ytxVideoView.seekTo(mVideoCurrentTime);
//            ytxVideoView.start();
//        }

       // ytxVideoView.onResume();

    }


    @Override
    protected void onRestart() {
        super.onRestart();
        YtxLog.d(TAG,"#### #### onRestart");
        ytxVideoView.seekTo(mVideoCurrentTime);
        ytxVideoView.start();
    }

    @Override
    protected void onPause() {
        super.onPause();
        YtxLog.d(TAG,"#### #### onPause");
        if(ytxVideoView.isPlaying()){
            ytxVideoView.onPause();
            mVideoCurrentTime = ytxVideoView.getCurrentPosition();
        }


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
      //  FileExplorerEvents.getBus().unregister(this);
        ytxVideoView.onDestroy();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        YtxLog.d(TAG,"#### #### onConfigurationChanged");
        doConfigChanged(newConfig);
    }

    private void doConfigChanged(Configuration newConfig) {
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            setUpLandLandscapeLayout();
            hideTitleBar(true);
        } else {
            setupPortraitLayout();
            hideTitleBar(false);
        }
    }

    private void setUpLandLandscapeLayout(){
        isFullScreen = true;
        findViewById(R.id.llInputSubPath).setVisibility(View.GONE);
        findViewById(R.id.llInputVideoPath).setVisibility(View.GONE);
        findViewById(R.id.llPlayFullScreen).setVisibility(View.GONE);

    }

    private void setupPortraitLayout(){
        isFullScreen = false;
        findViewById(R.id.llInputSubPath).setVisibility(View.VISIBLE);
        findViewById(R.id.llInputVideoPath).setVisibility(View.VISIBLE);
        findViewById(R.id.llPlayFullScreen).setVisibility(View.VISIBLE);
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
            case R.id.btAddSub:
                isAddSub = true;
                intent = new Intent(this, FileExplorerSubTitleActivity.class);
                startActivityForResult(intent, ActivityResultConst.FILE_EXPLORER_SUBTITLE_REQUEST_CODE);
                break;
            case R.id.ivDragVideo:
                actvFileNameVideo.showDropDown();
                break;
            case R.id.btPlay:
                PreferenceUtil.getInstance().putString(FILE_NAME,actvFileNameVideo.getText().toString().trim());
                fileName = actvFileNameVideo.getText().toString().trim();
                subtitles = actvFileNameSub.getText().toString().trim();
                ytxVideoView.onDestroy();
                if(!TextUtils.isEmpty(subtitles)){
                    ytxVideoView.setSubtitles(subtitles);
                }
                ytxVideoView.setVideoPath(fileName);
                ytxVideoView.start();

                break;
            case R.id.btFullScreen:
                enterFullScreen();
                break;
        }
    }


    @Override
    public void onBackPressed() {
        if (isFullScreen) {
            quitFullScreen();
            return;
        }
        super.onBackPressed();
    }

    private void enterFullScreen() {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    private void quitFullScreen() {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(requestCode == ActivityResultConst.FILE_EXPLORER_SUBTITLE_REQUEST_CODE && resultCode == RESULT_OK){
            String subtitles = data.getStringExtra(KeyConst.FILE_EXPLORER_SUBTITLE);
            actvFileNameSub.setText(subtitles);
        }
    }

    private static class AutoCompleteAdapter extends BaseAdapter implements Filterable {

        private ArrayFilter mFilter;

        private ArrayList<String> contacts;

        private Context context;

        private ArrayList<String> mUnfilteredData;
        AutoCompleteAdapter(Context context,ArrayList<String> contacts){
            this.context = context;
            this.contacts = contacts;
        }

        @Override
        public int getCount() {
            return contacts == null ? 0 : contacts.size();
        }

        @Override
        public Object getItem(int position) {
            return contacts.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            View view;
            ViewHolder holder;
            if(convertView==null){
                view = View.inflate(context, R.layout.item_file_name, null);

                holder = new ViewHolder();
                holder.tvAccount = (TextView) view.findViewById(R.id.text);
                holder.line = view.findViewById(R.id.line);
                view.setTag(holder);
            }else{
                view = convertView;
                holder = (ViewHolder) view.getTag();

            }

            holder.tvAccount.setText(contacts.get(position));

            return view;
        }

        @Override
        public Filter getFilter() {
            if (mFilter == null) {
                mFilter = new ArrayFilter();
            }
            return mFilter;
        }

        static class ViewHolder{
            public TextView tvAccount;
            public View line;
        }


        private class ArrayFilter extends Filter {


            @Override
            protected FilterResults performFiltering(CharSequence prefix) {
                FilterResults results = new FilterResults();

                if (mUnfilteredData == null) {
                    mUnfilteredData = new ArrayList<>(contacts);
                }

                if (prefix == null || prefix.length() == 0) {
                    ArrayList<String> list = mUnfilteredData;
                    results.values = list;
                    results.count = list.size();
                } else {
                    String prefixString = prefix.toString().toLowerCase();
                    ArrayList<String> unfilteredValues = mUnfilteredData;
                    int count = unfilteredValues.size();

                    ArrayList<String> newValues = new ArrayList<>(count);

                    for (int i = 0; i < count; i++) {
                        String contact = unfilteredValues.get(i);
                        if(!TextUtils.isEmpty(contact) && contact.startsWith(prefixString)){
                            newValues.add(contact);
                        }
                    }

                    results.values = newValues;
                    results.count = newValues.size();
                }

                return results;
            }

            @Override
            protected void publishResults(CharSequence constraint,
                                          FilterResults results) {
                contacts = (ArrayList<String>) results.values;
                if (results.count > 0) {
                    notifyDataSetChanged();
                } else {
                    notifyDataSetInvalidated();
                }
            }

        }
    }
}
