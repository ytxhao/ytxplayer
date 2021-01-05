package com.ytx.ican.ytxplayer.activity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;

import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentTransaction;

import com.squareup.otto.Subscribe;
import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.eventbus.FileExplorerEvents;
import com.ytx.ican.ytxplayer.fragment.FileListFragment;
import com.ytx.ican.ytxplayer.utils.Utils;

import java.io.File;
import java.io.IOException;

public class FileExplorerActivity extends BaseActivity implements FileListFragment.OnFragmentInteractionListener{

    private static final String TAG = "FileExplorerActivity";
    private final int requestCode = 321;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_explorer);
//        String ExternalStorageDirectory = Environment.getExternalStorageDirectory()
//                .getAbsolutePath();
//
//        doOpenDirectory(ExternalStorageDirectory, false);
        checkPermission();

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
            VideoMainActivity.intentTo(this, f.getPath(), f.getName());
           // finish();
        }
    }


    private void checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            boolean havePermissionDenied = false;
            int i = ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA);
            if (i != PackageManager.PERMISSION_GRANTED) {
                havePermissionDenied = true;
            }
            i = ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO);
            if (i != PackageManager.PERMISSION_GRANTED) {
                havePermissionDenied = true;
            }
            i = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
            if (i != PackageManager.PERMISSION_GRANTED) {
                havePermissionDenied = true;
            }
            i = ContextCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE);
            if (i != PackageManager.PERMISSION_GRANTED) {
                havePermissionDenied = true;
            }

            if (havePermissionDenied) {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_PHONE_STATE}, this.requestCode);
            } else {
                this.todo();
            }
        } else{
            this.todo();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == this.requestCode) {
            boolean allPermissionGranted = true;
            for (int i : grantResults) {
                if (i != PackageManager.PERMISSION_GRANTED) {
                    allPermissionGranted = false;
                    Log.d(TAG, "onRequestPermissionsResult: " + i);
                    break;
                }
            }
            if (allPermissionGranted) {
                this.todo();
            }
            Log.d(TAG, "onRequestPermissionsResult: " + allPermissionGranted);
        }
    }


    private void todo() {
        String filePath = Environment.getExternalStorageDirectory()
                .getAbsolutePath() + "/" ;

        //获取当前程序路径

        String getFilesDir=getApplicationContext().getFilesDir().getAbsolutePath();
        Utils.CopyAssets(this,"video",filePath);
        Utils.CopyAssets(this,"fonts",getFilesDir);
        String ExternalStorageDirectory = Environment.getExternalStorageDirectory()
                .getAbsolutePath();
        doOpenDirectory(ExternalStorageDirectory, false);
    }
}
