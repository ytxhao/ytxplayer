//
// Created by Administrator on 2017/8/7.
//

#include "../include/J4aBuild.h"


int J4A_LoadAll__catchAll(JNIEnv *env)
{
    int ret = 0;

    // load android.os.Build at very beginning
    J4A_LOAD_CLASS(android_os_Build);

    fail:
    return ret;
}

