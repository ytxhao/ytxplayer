//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_DECODER_VIDEO_H
#define YTXPLAYER_DECODER_VIDEO_H
#include "Decoder.h"
#include "frame_queue.h"

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
    FrameQueue *frameQueue;
    int frameQueueInitFinsh=0;
    struct SwsContext*	mConvertCtx;
    unsigned char *out_buffer_video;
    void setFrameQueue(FrameQueue *frameQueue){this->frameQueue = frameQueue;}
private:
    AVFrame*					mFrame;
    double						mVideoClock;
    AVRational timeBase ;
    AVRational frameRate;



    bool                        prepare();
    double 						synchronize(AVFrame *src_frame, double pts);
    bool                        decode(void* ptr);
    bool                        process(AVPacket *packet,int *i);
    static int					getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags);
    static void					releaseBuffer(struct AVCodecContext *c, AVFrame *pic);




};
#endif //YTXPLAYER_DECODER_VIDEO_H
