//
// Created by Administrator on 2016/9/2.
//

#define LOG_NDEBUG 0
#define LOG_TAG "YTX-PLAYER-JNI"

#include <string>
#include "ffinc.h"
#include "android_media_YtxMediaPlayer.h"
#include "YtxMediaPlayer.h"
// 获取数组的大小
#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

// 指定要注册的类，对应完整的java类名
//#define JNIREG_CLASS "com/example/aecm/MobileAEC"

#define JNIREG_CLASS "com/ytx/ican/media/player/YtxMediaPlayer"


// ----------------------------------------------------------------------------
const char *file_path;
static JavaVM *sVm;
/*
 * Throw an exception with the specified class and an optional message.
 */
int jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) {
        __android_log_print(ANDROID_LOG_ERROR,
                            LOG_TAG,
                            "Unable to find exception class %s",
                            className);
        return -1;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR,
                            LOG_TAG,
                            "Failed throwing '%s' '%s'",
                            className, msg);
    }
    return 0;
}


JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
    if (sVm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR,
                            LOG_TAG,
                            "Failed to obtain JNIEnv");
        return NULL;
    }
    return env;
}



// ----------------------------------------------------------------------------
struct fields_t {
    jfieldID    context;
    jfieldID    surface_texture;

    jmethodID   post_event;

    jmethodID   proxyConfigGetHost;
    jmethodID   proxyConfigGetPort;
    jmethodID   proxyConfigGetExclusionList;
};
static fields_t fields;

//static std

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaPlayerListener: public MediaPlayerListener
{
public:
    JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIMediaPlayerListener();
    // virtual void notify(int msg, int ext1, int ext2, const Parcel *obj = NULL);
    virtual void notify(int msg, int ext1, int ext2);
private:
    JNIMediaPlayerListener();
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
};

JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find android/media/MediaPlayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
    // remove global references
    JNIEnv *env = getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2)
{
//    JNIEnv *env = AndroidRuntime::getJNIEnv();
//    if (obj && obj->dataSize() > 0) {
//        jobject jParcel = createJavaParcelObject(env);
//        if (jParcel != NULL) {
//            Parcel* nativeParcel = parcelForJavaObject(env, jParcel);
//            nativeParcel->setData(obj->data(), obj->dataSize());
//            env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
//                                      msg, ext1, ext2, jParcel);
//        }
//    } else {
//        env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
//                                  msg, ext1, ext2, NULL);
//    }
//    if (env->ExceptionCheck()) {
//        ALOGW("An exception occurred while notifying an event.");
//        LOGW_EX(env);
//        env->ExceptionClear();
//    }
}

// ----------------------------------------------------------------------------

//YtxMediaPlayer* mPlayer;

JNIMediaPlayerListener *listener;



// ----------------------------------------------------------------------------

static YtxMediaPlayer* getMediaPlayer(JNIEnv* env, jobject thiz)
{
 //   Mutex::Autolock l(sLock);
    YtxMediaPlayer* const p = (YtxMediaPlayer*)env->GetIntField(thiz, fields.context);
    return (p);
}

static YtxMediaPlayer* setMediaPlayer(JNIEnv* env, jobject thiz, const YtxMediaPlayer* player)
{
    //Mutex::Autolock l(sLock);
    YtxMediaPlayer* old = (YtxMediaPlayer*)env->GetIntField(thiz, fields.context);
  //  if (player.get()) {
  //      player->incStrong((void*)setMediaPlayer);
  //  }
  //  if (old != 0) {
  //      old->decStrong((void*)setMediaPlayer);
  //  }
    env->SetIntField(thiz, fields.context, (int)player);
    return old;
}



JNIEXPORT void JNICALL android_media_player_native_init
        (JNIEnv *env, jclass obj)
{

    jclass clazz;

    clazz = env->FindClass(JNIREG_CLASS);
    if (clazz == NULL) {
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        return;
    }

    fields.surface_texture = env->GetFieldID(clazz, "mNativeSurfaceTexture", "I");
    if (fields.surface_texture == NULL) {
        return;
    }
//
//    clazz = env->FindClass("android/net/ProxyProperties");
//    if (clazz == NULL) {
//        return;
//    }
//
//    fields.proxyConfigGetHost =
//            env->GetMethodID(clazz, "getHost", "()Ljava/lang/String;");
//
//    fields.proxyConfigGetPort =
//            env->GetMethodID(clazz, "getPort", "()I");
//
//    fields.proxyConfigGetExclusionList =
//            env->GetMethodID(clazz, "getExclusionList", "()Ljava/lang/String;");

    ALOGI("avcodec_version=%d;avcodec_configuration=%s", avcodec_version(),avcodec_configuration());
}

JNIEXPORT void JNICALL android_media_player_native_setup
        (JNIEnv *env, jobject obj, jobject ytxMediaPlayer_weak_this)
{


    YtxMediaPlayer* mPlayer = new YtxMediaPlayer();
    ALOGI("native_setup mPlayer add = 0x%#x",(int)mPlayer);
    ALOGI("native_setup2 mPlayer add = 0x%#x",(int)mPlayer);
    // create new listener and give it to MediaPlayer
    listener = new JNIMediaPlayerListener(env, obj, ytxMediaPlayer_weak_this);
    mPlayer->setListener(listener);
    setMediaPlayer(env,obj,mPlayer);
}

JNIEXPORT void JNICALL android_media_player_native_finalize
        (JNIEnv *env, jobject obj)
{

}

JNIEXPORT void JNICALL android_media_player_native_message_loop
        (JNIEnv *env, jobject obj, jobject ytxMediaPlayer)
{

}



/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    _setGlSurface
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL android_media_player_setGlSurface
        (JNIEnv *env, jobject obj, jobject VideoGlSurfaceViewFFMPEG)
{

    ALOGI("android_media_player_setGlSurface IN VideoGlSurfaceViewFFMPEG=%d\n",VideoGlSurfaceViewFFMPEG);
    jclass glSurface_cls = env->GetObjectClass(VideoGlSurfaceViewFFMPEG); //或得Student类引用
    if(glSurface_cls == NULL)
    {
        ALOGI("GetObjectClass failed \n") ;
    }

//
//    jfieldID intFieldID = env->GetFieldID(glSurface_cls,"mWidth","I"); //获得得Student类的属性id
//    ALOGI("android_media_player_setGlSurface intFieldID=%d\n",intFieldID);
//
//    jint mWidth = env->GetIntField(VideoGlSurfaceViewFFMPEG , intFieldID);  //获得属性值
//    ALOGI("android_media_player_setGlSurface mWidth=%d\n",mWidth);

//-----------------------------------------------------------------
    jfieldID surfaceFieldID = env->GetFieldID(glSurface_cls,"mYUVTextures","[I"); //获得得Student类的属性id

    ALOGI("android_media_player_setGlSurface surfaceFieldID=%d\n",surfaceFieldID);
    jintArray jYUVTextures = (jintArray) env->GetObjectField(VideoGlSurfaceViewFFMPEG , surfaceFieldID);  //获得属性值

    ALOGI("android_media_player_setGlSurface jYUVTextures=%d\n",jYUVTextures);

    jint* mYUVTextures;
    mYUVTextures = env->GetIntArrayElements(jYUVTextures,NULL);//得到一个指向原始数据类型内容的指针
    jint length = env->GetArrayLength(jYUVTextures);//得到数组的长度

    int i=0;
    for(i=0;i<length;i++){

        ALOGI("android_media_player_setGlSurface mYUVTextures[%d]=%d\n",i,mYUVTextures[i]);

    }


    ALOGI("android_media_player_setGlSurface OUT\n");
}




/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    died
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_died
        (JNIEnv *env, jobject obj)
{

}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    disconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_disconnect
        (JNIEnv *env, jobject obj)
{

}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setDataSource
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL android_media_player_setDataSource
        (JNIEnv *env, jobject obj, jstring url)
{

    file_path = env->GetStringUTFChars(url,NULL);
    ALOGI("file_path=%s",file_path);
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    ALOGI("setDataSource mPlayer add 0x=%#x",(int)mPlayer);
    ALOGI("setDataSource mPlayer2 add 0x=%#x",(int)mPlayer);
    mPlayer->setDataSource(file_path);
  //  env->ReleaseStringUTFChars(url, file_path);
    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setDataSource
 * Signature: (IJJ)I
 */
//JNIEXPORT jint JNICALL android_media_player_setDataSource
//        (JNIEnv *env, jobject obj, jint, jlong, jlong);

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    prepare
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepare
        (JNIEnv *env, jobject obj)
{
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    mPlayer->prepare();
    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    prepareAsync
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepareAsync
        (JNIEnv *env, jobject obj)
{


    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_start
        (JNIEnv *env, jobject obj)
{
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    mPlayer->start();
    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_stop
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    pause
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_pause
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    isPlaying
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_isPlaying
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getVideoWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoWidth
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getVideoHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoHeight
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    seekTo
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_seekTo
        (JNIEnv *env, jobject obj, jint msec)
{
    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getCurrentPosition
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getCurrentPosition
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getDuration
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getDuration
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    reset
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_reset
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setLooping
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_setLooping
        (JNIEnv *env, jobject obj, jint loop)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    isLooping
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_isLooping
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setVolume
 * Signature: (FF)I
 */
JNIEXPORT jint JNICALL android_media_player_setVolume
        (JNIEnv *env, jobject obj, jfloat leftVolume, jfloat rightVolume)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setAudioSessionId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_setAudioSessionId
        (JNIEnv *env, jobject obj, jint sessionId)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getAudioSessionId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getAudioSessionId
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setAuxEffectSendLevel
 * Signature: (F)I
 */
JNIEXPORT jint JNICALL android_media_player_setAuxEffectSendLevel
        (JNIEnv *env, jobject obj, jfloat level)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    attachAuxEffect
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_attachAuxEffect
        (JNIEnv *env, jobject obj, jint effectId)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    setRetransmitEndpoint
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL android_media_player_setRetransmitEndpoint
        (JNIEnv *env, jobject obj, jstring addrString, jlong port)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    updateProxyConfig
 * Signature: (Ljava/lang/String;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL android_media_player_updateProxyConfig
        (JNIEnv *env, jobject obj, jstring host, jint port, jstring exclusionList)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    clear_l
 * Signature: ()V
 */
JNIEXPORT void JNICALL android_media_player_clear_l
        (JNIEnv *env, jobject obj)
{

}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    seekTo_l
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL android_media_player_seekTo_l
        (JNIEnv *env, jobject obj, jint msec)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    prepareAsync_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_prepareAsync_l
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getDuration_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getDuration_l
        (JNIEnv *env, jobject obj)
{

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    reset_l
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_reset_l
        (JNIEnv *env, jobject obj)
{

    return 0;
}


// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {

        {"native_init",      "()V",                              (void *)android_media_player_native_init},
        {"native_setup",        "(Ljava/lang/Object;)V",         (void *)android_media_player_native_setup},
        {"native_finalize",  "()V",                              (void *)android_media_player_native_finalize},
        {"native_message_loop", "(Ljava/lang/Object;)V",         (void *)android_media_player_native_message_loop},


        {"_setGlSurface", "(Ljava/lang/Object;)V",         (void *)android_media_player_setGlSurface},
        {"_died", "()V",         (void *)android_media_player_died},
        {"_setDataSource", "(Ljava/lang/String;)I",         (void *)android_media_player_setDataSource},
 //       {"native_message_loop", "(IJJ)I",         (void *)android_media_player_setDataSource},
        {"_prepare", "()I",         (void *)android_media_player_prepare},
        {"_prepareAsync", "()I",         (void *)android_media_player_prepareAsync},
        {"_start", "()I",         (void *)android_media_player_start},
        {"_stop", "()I",         (void *)android_media_player_stop},
        {"_pause", "()I",         (void *)android_media_player_pause},
        {"_isPlaying", "()I",         (void *)android_media_player_isPlaying},
        {"_getVideoWidth", "()I",         (void *)android_media_player_getVideoWidth},
        {"_getVideoHeight", "()I",         (void *)android_media_player_getVideoHeight},
        {"_seekTo", "(I)I",         (void *)android_media_player_seekTo},
        {"_getCurrentPosition", "()I",         (void *)android_media_player_getCurrentPosition},
        {"_getDuration", "()I",         (void *)android_media_player_getDuration},
        {"_reset", "()I",         (void *)android_media_player_reset},
        {"_setLooping", "(I)I",         (void *)android_media_player_setLooping},
        {"_isLooping", "()I",         (void *)android_media_player_isLooping},
        {"_setVolume", "(FF)I",         (void *)android_media_player_setVolume},
        {"_setAudioSessionId", "(I)I",         (void *)android_media_player_setAudioSessionId},
        {"_getAudioSessionId", "()I",         (void *)android_media_player_getAudioSessionId},
        {"_setAuxEffectSendLevel", "(F)I",         (void *)android_media_player_setAuxEffectSendLevel},
        {"_attachAuxEffect", "(I)I",         (void *)android_media_player_attachAuxEffect},
        {"_setRetransmitEndpoint", "(Ljava/lang/String;J)I",         (void *)android_media_player_setRetransmitEndpoint},
        {"_updateProxyConfig", "(Ljava/lang/String;ILjava/lang/String;)I",         (void *)android_media_player_updateProxyConfig},
        {"_clear_l", "()V",         (void *)android_media_player_clear_l},
        {"_seekTo_l", "(I)I",         (void *)android_media_player_seekTo_l},
        {"_prepareAsync_l", "()I",         (void *)android_media_player_prepareAsync_l},
        {"_getDuration_l", "()I",         (void *)android_media_player_getDuration_l},
        {"_reset_l", "()I",         (void *)android_media_player_reset_l}



};



// 注册native方法到java中
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}


int register_android_media_player_ytx(JNIEnv *env)
{
    // 调用注册方法
    return registerNativeMethods(env, JNIREG_CLASS,
                                 gMethods, NELEM(gMethods));
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    sVm = vm;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);


    if (register_android_media_player_ytx(env) < 0) {
        ALOGE("ERROR: YTX mediaPlayer native registration failed\n");
        goto bail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}