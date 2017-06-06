//
// Created by Administrator on 2017/3/15.
//
#define LOG_NDEBUG 0
#define TAG "YTX-FONT-SEARCH-JNI"
#include <stddef.h>
#include <ytxplayer/ALog-priv.h>
#include <assert.h>
#include <jni.h>
#include "fontconfigpath.h"




// 获取数组的大小
#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif
#define JNIREG_FONT_CFG_CLASS "com/ytx/ican/ytxplayer/utils/FontSearchConfig"
static JavaVM *sVm;
//
static char *font_search_path=NULL;

void set_font_search_path(char * path){
    font_search_path = path;
}

char* get_font_search_path(){
    return font_search_path;
}


JNIEXPORT void JNICALL android_media_player_FontSearchConfig_setFontSearchPath
        (JNIEnv *env, jclass clazz , jstring path){
    const char *dir_path = (*env)->GetStringUTFChars(env,path,NULL);
    set_font_search_path(dir_path);

}


JNIEXPORT jstring JNICALL android_media_player_FontSearchConfig_getFontSearchPath
        (JNIEnv *env, jclass clazz){

    //ALOGI("getFontSearchPath font_search_path 12356=%s",get_font_search_path());
    return (*env)->NewStringUTF(env, (const char*)get_font_search_path);
}


// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {

        {"native_set_font_search_path",      "(Ljava/lang/String;)V",                      (void *)android_media_player_FontSearchConfig_setFontSearchPath},
        {"native_get_font_search_path",        "()Ljava/lang/String;",                (void *)android_media_player_FontSearchConfig_getFontSearchPath},

};





// 注册native方法到java中
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = (*env)->FindClass(env,className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env,clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}


int register_android_media_player_font_cfg(JNIEnv *env)
{
    // 调用注册方法
    return registerNativeMethods(env, JNIREG_FONT_CFG_CLASS,
                                 gMethods, NELEM(gMethods));
}


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv *env = NULL;
    jint result = -1;
    sVm = vm;
    ALOGI("INF font cfg: sVm=%d\n", sVm);
    if ((*sVm)->GetEnv(sVm,(void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR renderer: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);


    if (register_android_media_player_font_cfg(env) < 0) {
        ALOGE("ERROR: mediaPlayer font cfg native registration failed\n");
        goto bail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}