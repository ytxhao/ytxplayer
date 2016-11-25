
#include <string>
#include "ytxplayer/native-lib.h"

jstring
Java_com_ytx_ican_ytxplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    av_register_all();
    avformat_network_init();
    AVFormatContext *pFormatCtx;
    pFormatCtx = avformat_alloc_context();
    char str[1024];
    sprintf(str, "avcodec_version=%d;avcodec_configuration=%s", avcodec_version(),avcodec_configuration());
    return env->NewStringUTF(str);
}
