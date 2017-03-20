package com.ytx.ican.ytxplayer.base;


import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.widget.Toast;

import com.ytx.ican.ytxplayer.activity.BaseActivity;



public class ActivityHelper {

    private BaseActivity mActivity;

    public ActivityHelper(BaseActivity context) {
        this.mActivity = context;
    }



    public void showMessage(int id) {
        Toast.makeText(mActivity, id, Toast.LENGTH_SHORT).show();
    }

    public void showMessage(String msg) {
        Toast.makeText(mActivity, msg, Toast.LENGTH_SHORT).show();
    }

    public void showMessageLengthLONG(int id) {
        Toast.makeText(mActivity, id, Toast.LENGTH_LONG).show();
    }

    public void showMessageLengthLONG(String msg) {
        Toast.makeText(mActivity, msg, Toast.LENGTH_LONG).show();
    }


    public boolean isWifiNetwork() {
        ConnectivityManager connMgr = (ConnectivityManager)mActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();
        if (networkInfo != null && networkInfo.isConnected()) {
            if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI) {
                return true;
            }
        }
        return false;
    }

    public boolean isNonWifiNetwork(){
        ConnectivityManager connMgr = (ConnectivityManager)mActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connMgr.getActiveNetworkInfo();
        if (networkInfo != null && networkInfo.isConnected()) {
            if (networkInfo.getType() == ConnectivityManager.TYPE_MOBILE) {
                return true;
            }
        }
        return false;
    }


    public boolean isNetAvailable(){
        ConnectivityManager manager = (ConnectivityManager)mActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        if (manager != null) {
            NetworkInfo networkInfo = manager.getActiveNetworkInfo();
            if (networkInfo != null && networkInfo.isAvailable()) {
                return true;
            }
        }
        return false;
    }

}
