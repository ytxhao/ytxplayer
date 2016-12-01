//
// Created by Administrator on 2016/12/1.
//

#ifndef YTXPLAYER_INPUTSTREAM_H
#define YTXPLAYER_INPUTSTREAM_H

#include "ffinc.h"
class InputStream{
public:
    AVStream *st;
    AVCodecContext *dec_ctx;
};

#endif //YTXPLAYER_INPUTSTREAM_H
