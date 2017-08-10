//
// Created by Administrator on 2017/7/18.
//

#ifndef YTXPLAYER_J4ABUILD_H
#define YTXPLAYER_J4ABUILD_H
#ifdef __cplusplus
extern "C" {
#endif

#include "J4aBase.h"

jint J4AC_android_os_Build__VERSION__SDK_INT__get(JNIEnv *env);

jint J4AC_android_os_Build__VERSION__SDK_INT__get__catchAll(JNIEnv *env);

void J4AC_android_os_Build__VERSION__SDK_INT__set(JNIEnv *env, jint value);

void J4AC_android_os_Build__VERSION__SDK_INT__set__catchAll(JNIEnv *env, jint value);

int J4A_loadClass__J4AC_android_os_Build(JNIEnv *env);

#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_J4ABUILD_H
