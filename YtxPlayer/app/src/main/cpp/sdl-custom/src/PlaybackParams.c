//
// Created by Administrator on 2017/8/7.
//

#include "../include/PlaybackParams.h"

typedef struct J4AC_android_media_PlaybackParams {
    jclass id;

    jmethodID method_setSpeed;
} J4AC_android_media_PlaybackParams;
static J4AC_android_media_PlaybackParams class_J4AC_android_media_PlaybackParams;

jobject J4AC_android_media_PlaybackParams__setSpeed(JNIEnv *env, jobject thiz, jfloat speed)
{
    return (*env)->CallObjectMethod(env, thiz, class_J4AC_android_media_PlaybackParams.method_setSpeed, speed);
}

jobject J4AC_android_media_PlaybackParams__setSpeed__catchAll(JNIEnv *env, jobject thiz, jfloat speed)
{
    jobject ret_object = J4AC_android_media_PlaybackParams__setSpeed(env, thiz, speed);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_android_media_PlaybackParams__setSpeed__asGlobalRef__catchAll(JNIEnv *env, jobject thiz, jfloat speed)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_android_media_PlaybackParams__setSpeed__catchAll(env, thiz, speed);
    if (J4A_ExceptionCheck__catchAll(env) || !local_object) {
        ret_object = NULL;
        goto fail;
    }

    ret_object = J4A_NewGlobalRef__catchAll(env, local_object);
    if (!ret_object) {
        ret_object = NULL;
        goto fail;
    }

    fail:
    J4A_DeleteLocalRef__p(env, &local_object);
    return ret_object;
}

int J4A_loadClass__J4AC_android_media_PlaybackParams(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_android_media_PlaybackParams.id != NULL)
        return 0;

    api_level = J4A_GetSystemAndroidApiLevel(env);

    if (api_level < 23) {
        J4A_ALOGW("J4ALoader: Ignore: '%s' need API %d\n", "android.media.PlaybackParams", api_level);
        goto ignore;
    }

    sign = "android/media/PlaybackParams";
    class_J4AC_android_media_PlaybackParams.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_android_media_PlaybackParams.id == NULL)
        goto fail;

    class_id = class_J4AC_android_media_PlaybackParams.id;
    name     = "setSpeed";
    sign     = "(F)Landroid/media/PlaybackParams;";
    class_J4AC_android_media_PlaybackParams.method_setSpeed = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_android_media_PlaybackParams.method_setSpeed == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "android.media.PlaybackParams");
    ignore:
    ret = 0;
    fail:
    return ret;
}