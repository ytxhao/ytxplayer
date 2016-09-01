//
// Created by yuhao on 16-9-1.
//

#ifndef YTXPLAYER_NATIVE_LIB_H
#define YTXPLAYER_NATIVE_LIB_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif

jstring  Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */);



#endif //YTXPLAYER_NATIVE_LIB_H
