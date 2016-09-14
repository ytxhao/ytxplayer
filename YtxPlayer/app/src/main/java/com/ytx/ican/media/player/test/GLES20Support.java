package com.ytx.ican.media.player.test;


import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;


public class GLES20Support {

    public static boolean detectOpenGLES20(Context context) {
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return (info.reqGlEsVersion >= 0x20000);
    }
}
