package com.ytx.ican.ytxplayer.activity;


import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.base.ActivityHelper;

import java.util.ArrayList;

public class BaseActivity extends AppCompatActivity {
    private ActivityHelper mHelper;
    private View loadingView;
    protected Handler mHandler;
    private Runnable mDismissLoadingRunnable = new Runnable() {

        @Override
        public void run() {
            dismissAllLoading();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_base);
    }

    public ActivityHelper getHelper() {
        if (mHelper == null) {
            mHelper = new ActivityHelper(this);
        }
        return mHelper;
    }

    public Handler getHandler(){
        initHandler();
        return mHandler;
    }

    private void initHandler() {
        if (mHandler == null) {
            mHandler = new Handler(getMainLooper());
        }
    }


    public void showLoading(int loadingId) {
        showLoading(loadingId, null);
        initHandler();
        mHandler.removeCallbacks(mDismissLoadingRunnable);
        mHandler.postDelayed(mDismissLoadingRunnable, 20 * 1000);
    }

    public void showLoading() {
        showLoading(0);
    }

    public void showLoading(int loadingId, String message) {
        if (loadingView == null) {
            loadingView = initLoadingView();
        }
        if (message != null) {
            TextView tv = (TextView) loadingView.findViewById(R.id.tv_loading);
            if (tv != null) {
                tv.setText(message);
                tv.setVisibility(View.VISIBLE);
            }
        }
        ArrayList<Integer> list = (ArrayList<Integer>) loadingView.getTag();
        list.add(loadingId);
        if (loadingView.getParent() != null) {
            return;
        }
        ViewGroup vg = (ViewGroup) getWindow().getDecorView();
        vg.addView(loadingView);
    }

    protected View initLoadingView() {
        View loadingView = getLayoutInflater().inflate(R.layout.progress_loading, null);
        loadingView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return true;
            }
        });
        ArrayList<Integer> list = new ArrayList<Integer>();
        loadingView.setTag(list);
        return loadingView;
    }


    public void dismissLoading(int loadingId) {
        if (loadingView != null && loadingView.getParent() != null) {
            ArrayList<Integer> list = (ArrayList<Integer>) loadingView.getTag();
            list.remove(Integer.valueOf(loadingId));
            if (list.isEmpty()) {
                ((ViewGroup) loadingView.getParent()).removeView(loadingView);
            }
        }
    }

    public void dismissLoading() {
        dismissLoading(0);
    }


    public void dismissAllLoading() {
        if (loadingView != null && loadingView.getParent() != null) {
            ArrayList<Integer> list = (ArrayList<Integer>) loadingView.getTag();
            list.clear();
            ((ViewGroup) loadingView.getParent()).removeView(loadingView);
        }
    }

    public boolean isLoading() {
        if (loadingView != null && loadingView.getParent() != null) {
            return true;
        }
        return false;
    }

    public void doInUI(Runnable runnable){
        doInUI(runnable, 0);
    }

    public void doInUI(Runnable runnable, long delayMillis){
        initHandler();
        mHandler.postDelayed(runnable, delayMillis);
    }


    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }


    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
