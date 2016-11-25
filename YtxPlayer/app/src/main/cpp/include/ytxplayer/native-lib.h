//
// Created by yuhao on 16-9-1.
//

#ifndef YTXPLAYER_NATIVE_LIB_H
#define YTXPLAYER_NATIVE_LIB_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __STDC_CONSTANT_MACROS

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

jstring  Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */);

#ifdef __cplusplus
}
#endif


#endif //YTXPLAYER_NATIVE_LIB_H
