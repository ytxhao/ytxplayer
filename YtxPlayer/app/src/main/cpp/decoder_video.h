//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_DECODER_VIDEO_H
#define YTXPLAYER_DECODER_VIDEO_H
#include "decoder.h"

typedef void (*VideoDecodingHandler) (AVFrame*,double);
typedef void (*VideoDecodeFinishHandler) ();
class DecoderVideo : public IDecoder
{
public:
    DecoderVideo(InputStream* stream);
    ~DecoderVideo();

    VideoDecodingHandler		onDecode;
    VideoDecodeFinishHandler    onDecodeFinish;
    int isFinish;
private:
    AVFrame*					mFrame;
    double						mVideoClock;

    bool                        prepare();
    double 						synchronize(AVFrame *src_frame, double pts);
    bool                        decode(void* ptr);
    bool                        process(AVPacket *packet,int *i);
    static int					getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags);
    static void					releaseBuffer(struct AVCodecContext *c, AVFrame *pic);
};
#endif //YTXPLAYER_DECODER_VIDEO_H
