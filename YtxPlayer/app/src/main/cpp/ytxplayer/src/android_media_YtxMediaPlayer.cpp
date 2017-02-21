//
// Created by Administrator on 2016/9/2.
//

#define LOG_NDEBUG 0
#define TAG "YTX-PLAYER-JNI"
#include "ytxplayer/ALog-priv.h"
#include <string>
#include <ytxplayer/gl_engine.h>
#include "ytxplayer/ffinc.h"
#include "ytxplayer/android_media_YtxMediaPlayer.h"
#include "ytxplayer/YtxMediaPlayer.h"
// 获取数组的大小
#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

// 指定要注册的类，对应完整的java类名
//#define JNIREG_CLASS "com/example/aecm/MobileAEC"

#define JNIREG_CLASS "com/ytx/ican/media/player/pragma/YtxMediaPlayer"

JavaVM *sVm;
// ----------------------------------------------------------------------------
/*
 * Throw an exception with the specified class and an optional message.
 */
int jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) {
        __android_log_print(ANDROID_LOG_ERROR,
                            TAG,
                            "Unable to find exception class %s",
                            className);
        return -1;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR,
                            TAG,
                            "Failed throwing '%s' '%s'",
                            className, msg);
    }
    return 0;
}


JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
 //   ALOGE("ERROR12: sVm=%d\n",sVm);
    if (sVm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR,
                            TAG,
                            "Failed to obtain JNIEnv");
        return NULL;
    }
    return env;
}



// ----------------------------------------------------------------------------
struct fields_t {
    jfieldID    context;
    jfieldID    surface_texture;

    jfieldID    native_player;

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
    pthread_mutex_t     		mLock;
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
};

JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    ALOGE("JNIMediaPlayerListener IN");
    pthread_mutex_init(&mLock, NULL);
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
    ALOGE("JNIMediaPlayerListener OUT mClass=%x mObject=%x clazz=%x\n",mClass,mObject,clazz);
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
    // remove global references
    JNIEnv *env = getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
    pthread_mutex_destroy(&mLock);
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2)
{

    ALOGI("JNIMediaPlayerListener::notify IN\n");
    pthread_mutex_lock(&mLock);
    JNIEnv *env = NULL;
    if(getpid() != gettid()){
        sVm->AttachCurrentThread(&env, NULL);
    }else{
        env = getJNIEnv();
    }

    ALOGI("notify pthread_self:%lu,getpid:%lu,gettid:%lu\n", (long)pthread_self(), (long)getpid(),(long)gettid());

    ALOGI("JNIMediaPlayerListener::notify fields.post_event=%d mClass=%x\n",fields.post_event,mClass);
    env->CallStaticVoidMethod(mClass,fields.post_event,mObject,msg, ext1, ext2, NULL);

    if (env->ExceptionCheck()) {
        ALOGW("An exception occurred while notifying an event.");
        env->ExceptionClear();
    }

    if(getpid() != gettid()){
        sVm->DetachCurrentThread();
    }
    pthread_mutex_unlock(&mLock);
    ALOGI("JNIMediaPlayerListener::notify OUT\n");
}

// ----------------------------------------------------------------------------

//YtxMediaPlayer* mPlayer;

//JNIMediaPlayerListener *listener;



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
        (JNIEnv *env, jclass mClazz)
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
    ALOGI("android_media_player_native_init fields.post_event=%d",fields.post_event);
    if (fields.post_event == NULL) {
        return;
    }

    fields.surface_texture = env->GetFieldID(clazz, "mNativeSurfaceTexture", "I");
    if (fields.surface_texture == NULL) {
        return;
    }
    ALOGI("native_init pthread_self:%lu,getpid:%lu,gettid:%lu\n", pthread_self(), getpid(),gettid());
    ALOGI("avcodec_version=%d;avcodec_configuration=%s", avcodec_version(),avcodec_configuration());
}

JNIEXPORT void JNICALL android_media_player_native_setup
        (JNIEnv *env, jobject obj, jobject ytxMediaPlayer_weak_this)
{


    YtxMediaPlayer* mPlayer = new YtxMediaPlayer();
    ALOGI("native_setup mPlayer add = 0x%#x",(int)mPlayer);

    // create new listener and give it to MediaPlayer
    JNIMediaPlayerListener* listener = new JNIMediaPlayerListener(env, obj, ytxMediaPlayer_weak_this);
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



void android_media_player_notifyRenderFrame(jobject obj)
{
    ALOGI("android_media_player_notifyRenderFrame IN\n");

  //  ALOGI("android_media_player_notifyRenderFrame VideoGlSurfaceViewObj=%d\n",VideoGlSurfaceViewObj);
    JNIEnv *env = NULL;
    sVm->AttachCurrentThread(&env, NULL);

//----------------------------------------------

    // 得到jclass
    jclass jclazz = env->GetObjectClass(obj);
     ALOGI("android_media_player_notifyRenderFrame jclazz=%d\n",jclazz);
    // 得到方法ID
    jmethodID jmtdId = env->GetMethodID(jclazz, "requestRender", "()V");
     ALOGI("android_media_player_notifyRenderFrame jmtdId=%d\n",jmtdId);
    // 调用方法
    env->CallVoidMethod(obj, jmtdId);

    sVm->DetachCurrentThread();
    ALOGI("android_media_player_notifyRenderFrame OUT\n");

}


/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayerTest
 * Method:    _setGlSurface
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL android_media_player_setGlSurface
        (JNIEnv *env, jobject obj, jobject mVideoGlSurfaceView)
{
    ALOGI("android_media_player_setGlSurface IN\n");
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    mPlayer->mVideoStateInfo->VideoGlSurfaceViewObj  = env->NewGlobalRef(mVideoGlSurfaceView);

    // 得到jclass
    jclass jclazz = env->GetObjectClass(mVideoGlSurfaceView);
    ALOGI("android_media_player_setGlSurface jclazz=%d\n",jclazz);
    // 得到方法ID
    jmethodID jmtdId = env->GetMethodID(jclazz, "getRenderer", "()Lcom/ytx/ican/media/player/render/GraphicRenderer;");
    ALOGI("android_media_player_setGlSurface jmtdId=%d\n",jmtdId);
    // 调用方法
    jobject GraphicRendererObj = env->CallObjectMethod(mVideoGlSurfaceView, jmtdId);

    ALOGI("android_media_player_setGlSurface jmtdId=%d GraphicRendererObj=%#x\n",jmtdId,GraphicRendererObj);
    mPlayer->mVideoStateInfo->GraphicRendererObj = env->NewGlobalRef(GraphicRendererObj);
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

    const char *file_path = env->GetStringUTFChars(url,NULL);
    ALOGI("file_path=%s",file_path);
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    ALOGI("setDataSource mPlayer add 0x=%#x",(int)mPlayer);
    mPlayer->setDataSource(file_path);
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
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    mPlayer->prepareAsync();
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
    ALOGI("android_media_player_stop");
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    mPlayer->stop();

    return 0;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    release
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_release
        (JNIEnv *env, jobject obj)
{
    ALOGI("android_media_player_release");
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    mPlayer->release();

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
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    mPlayer->pause();
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


    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    //mPlayer->isPlaying();
    return mPlayer->isPlaying();
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getVideoWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoWidth
        (JNIEnv *env, jobject obj)
{
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);

    return mPlayer->getVideoWidth();
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getVideoHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getVideoHeight
        (JNIEnv *env, jobject obj)
{
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    return mPlayer->getVideoHeight();
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    seekTo
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL android_media_player_seekTo
        (JNIEnv *env, jobject obj, jint msec)
{

    // jlong <==> long long 使用%lld输出
    ALOGI("android_media_player_seekTo msec=%d\n",msec);
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    mPlayer->seekTo(msec);
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
    jint ret = 0;
    ALOGI("android_media_player_getCurrentPosition IN\n");
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    ret = mPlayer->getCurrentPosition();
    ALOGI("android_media_player_getCurrentPosition OUT ret=%d\n",ret);
    return ret;
}

/*
 * Class:     com_ytx_ican_media_player_YtxMediaPlayer
 * Method:    getDuration
 * Signature: ()I
 */
JNIEXPORT jint JNICALL android_media_player_getDuration
        (JNIEnv *env, jobject obj)
{

    jint ret = 0;
    ALOGI("android_media_player_getDuration IN\n");
    YtxMediaPlayer* mPlayer =  getMediaPlayer(env,obj);
    ret = mPlayer->getDuration();
    ALOGI("android_media_player_getDuration OUT ret=%d\n",ret);
    return ret;
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
        {"_release", "()I",         (void *)android_media_player_release},
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
    ALOGI("main11 tid:%u,pid:%u\n", (unsigned)pthread_self(),
          (unsigned)getpid());
    ALOGI("ERROR11: sVm=%d\n",sVm);
    if (sVm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
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