package com.ytx.ican;


import android.app.Application;
import android.content.Context;
import android.util.DisplayMetrics;


import com.ytx.ican.media.player.pragma.YtxLog;
import com.ytx.ican.ytxplayer.utils.ScreenUtil;



public class VideoApplication extends Application {

    private static String TAG = "VideoApplication";

    public static Context context;



    @Override
    public void onCreate() {
        super.onCreate();

        //读取屏幕相关信息
        DisplayMetrics dm = getResources().getDisplayMetrics();
        ScreenUtil.screenWidth = dm.widthPixels < dm.heightPixels?dm.widthPixels:dm.heightPixels;
        ScreenUtil.screenHeight = dm.widthPixels > dm.heightPixels?dm.widthPixels:dm.heightPixels;
        ScreenUtil.density = dm.density;
        ScreenUtil.densityDpi = dm.densityDpi;
        YtxLog.d(TAG, "screen width:" + ScreenUtil.screenWidth
                        + ", height:" + ScreenUtil.screenHeight
                        + ", density:" + ScreenUtil.density
                        + ", densityDpi:" + ScreenUtil.densityDpi);

    }

    @Override
    public void onTerminate() {
        super.onTerminate();
    }

}

