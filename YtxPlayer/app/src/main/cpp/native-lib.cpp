
#include <string>
#include "native-lib.h"
jstring
Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    av_register_all();
    avformat_network_init();
    AVFormatContext *pFormatCtx;
    pFormatCtx = avformat_alloc_context();
    return env->NewStringUTF(hello.c_str());
}
