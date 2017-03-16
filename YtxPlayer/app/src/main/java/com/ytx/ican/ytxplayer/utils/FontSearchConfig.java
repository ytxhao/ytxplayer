package com.ytx.ican.ytxplayer.utils;

/**
 * Created by Administrator on 2017/3/15.
 */

public class FontSearchConfig {


    static {
        System.loadLibrary("fontconfigpath");
    }

    public static void setFontSearchPath(String path){
        native_set_font_search_path(path);
    }

    public static String getFontSearchPath(){
        return native_get_font_search_path();
    }

    private static native void native_set_font_search_path(String path);
    private static native String native_get_font_search_path();

}
