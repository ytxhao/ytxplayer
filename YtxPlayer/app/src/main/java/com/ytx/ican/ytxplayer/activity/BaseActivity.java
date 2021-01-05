package com.ytx.ican.ytxplayer.activity;


import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;


import androidx.appcompat.app.AppCompatActivity;

import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.R;
import com.ytx.ican.ytxplayer.base.ActivityHelper;
import com.ytx.ican.ytxplayer.utils.ScreenUtil;

import java.util.ArrayList;
import java.util.List;

public class BaseActivity extends AppCompatActivity {

    private ActivityHelper mHelper;
    private View loadingView;
    protected Handler mHandler;

    protected boolean needTransparent = false;

    private Runnable mDismissLoadingRunnable = new Runnable() {

        @Override
        public void run() {
            dismissAllLoading();
        }
    };


    // 当前activity是否是前台运行状态
    public boolean isForegroundRunning = false;

    // 当前app是否是前台状态
    private static boolean isActive = false;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        YtxLog.D(getClassName() + ":onCreate");
        isForegroundRunning = true;
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


    /**
     * 确保mHandler非空
     */
    private void initHandler() {
        if (mHandler == null) {
            mHandler = new Handler(getMainLooper());
        }
    }

    /**
     * 显示loading视图
     *
     * @param loadingId 与取消loading时调用dismissLoading(id)的参数对应
     *                  <p>调用了多少次showLoading(id) 就必须对应的调用多少次 dismissLoding(id)。
     *                  <p>不同的耗时操作应有不同的loadingId
     */
    public void showLoading(int loadingId) {
        showLoading(loadingId, null);
        initHandler();
        mHandler.removeCallbacks(mDismissLoadingRunnable);
        mHandler.postDelayed(mDismissLoadingRunnable, 20 * 1000);
    }

    /**
     * 调用了多少次该方法 就必须对应的调用多少次 dismissLoding()。
     */
    public void showLoading() {
        showLoading(0);
    }

    /**
     * @param message loading 提示文字
     */
    public void showLoading(int loadingId, String message) {
        if (loadingView == null) {
            loadingView = initLoadingView();
        }
        if (message != null) {
            TextView tv = (TextView) loadingView.findViewById(R.id.loadingMessage);
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

    /**
     * 初始化loading视图，可自定义重写，
     */
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

    /**
     * 取消对应id的loading
     *
     * @param loadingId 和调用showLoading传的id对应
     */
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

    /**
     * 取消所有loading
     */
    public void dismissAllLoading() {
        if (loadingView != null && loadingView.getParent() != null) {
            ArrayList<Integer> list = (ArrayList<Integer>) loadingView.getTag();
            list.clear();
            ((ViewGroup) loadingView.getParent()).removeView(loadingView);
        }
    }

    /**
     * 是否在加载
     *
     * @return
     */
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

    public void toActivity(Class<? extends BaseActivity> clazz) {
        startActivity(new Intent(this, clazz));
    }

    @Override
    protected void onResume() {
        super.onResume();
        YtxLog.D(getClassName() + ":onResume");
        isForegroundRunning = true;

        if(needTransparent) {
            ScreenUtil.makeStatusBarTransparent(this, true);
        }

        if (!isActive) {
            //app 从后台唤醒，进入前台
            YtxLog.D("----app is in foreground----");
            isActive = true;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        YtxLog.D(getClassName() + ":onPause");
    }

    @Override
    protected void onStop() {
        super.onStop();
        YtxLog.D(getClassName() + ":onStop");
        isForegroundRunning = false;

        if (!isAppOnForeground()) {
            //app 进入后台
            YtxLog.D("----app is in background----");
            isActive = false;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        YtxLog.D(getClassName() + ":onDestroy");
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        YtxLog.D(getClassName() + ":onRestoreInstanceState");
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        YtxLog.D(getClassName() + ":onSaveInstanceState");
        isForegroundRunning = false;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        YtxLog.D(getClassName() + ":onActivityResult");
    }

    @Override
    public void onBackPressed() {
        if (loadingView == null || loadingView.getParent() == null) {
            super.onBackPressed();
            return;
        }
        ArrayList<Integer> list = (ArrayList<Integer>) loadingView.getTag();
        list.clear();
        ((ViewGroup) loadingView.getParent()).removeView(loadingView);
    }

    public <V extends View> V findView(int id) {
        return (V) findViewById(id);
    }

    private String getClassName(){
        return getClass().getSimpleName();
    }



    /**
     * 程序是否在前台运行
     *
     * @return
     */
    private boolean isAppOnForeground() {
        // Returns a list of application processes that are running on the device

        ActivityManager activityManager = (ActivityManager) getApplicationContext().getSystemService(Context.ACTIVITY_SERVICE);
        String packageName = getApplicationContext().getPackageName();

        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager
                .getRunningAppProcesses();
        if (appProcesses == null)
            return false;

        for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
            // The name of the process that this object is associated with.
            if (appProcess.processName.equals(packageName)
                    && appProcess.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                return true;
            }
        }

        return false;
    }

    public boolean isNeedTransparent() {
        return needTransparent;
    }

    public void setNeedTransparent(boolean needTransparent) {
        this.needTransparent = needTransparent;
    }
}
