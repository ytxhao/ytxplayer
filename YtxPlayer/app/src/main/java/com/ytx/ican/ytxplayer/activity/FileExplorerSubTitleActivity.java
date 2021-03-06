package com.ytx.ican.ytxplayer.activity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.text.TextUtils;

import com.squareup.otto.Subscribe;
import com.ytx.ican.media.player.pragma.AndroidMediaPlayer;
import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.constants.KeyConst;
import com.ytx.ican.ytxplayer.eventbus.FileExplorerEvents;
import com.ytx.ican.ytxplayer.fragment.FileListFragment;

import java.io.File;
import java.io.IOException;

public class FileExplorerSubTitleActivity extends BaseActivity implements FileListFragment.OnFragmentInteractionListener{

    private static final String TAG = "FileExplorerSubTitleActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_explorer);
        String ExternalStorageDirectory = Environment.getExternalStorageDirectory()
                .getAbsolutePath();

        doOpenDirectory(ExternalStorageDirectory, false);


    }

    @Override
    protected void onResume() {
        super.onResume();
        FileExplorerEvents.getBus().register(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        FileExplorerEvents.getBus().unregister(this);
    }

    private void doOpenDirectory(String externalStorageDirectory, boolean addToBackStack) {
        Fragment newFragment = FileListFragment.newInstance(externalStorageDirectory,"");
        FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();

        transaction.replace(R.id.body, newFragment);

        if (addToBackStack)
            transaction.addToBackStack(null);
        transaction.commit();

    }


    @Override
    public void onFragmentInteraction(Uri uri) {
        YtxLog.d(TAG,"onFragmentInteraction uri"+uri);
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
            String path = f.toString();
            doOpenDirectory(path, true);
        } else if (f.exists()) {
            YtxLog.d(TAG,f.getPath()+" "+f.getName());
            Intent intent = new Intent();
            intent.putExtra(KeyConst.FILE_EXPLORER_SUBTITLE, f.getPath());
            setResult(RESULT_OK, intent);
            finish();
        }
    }
}
