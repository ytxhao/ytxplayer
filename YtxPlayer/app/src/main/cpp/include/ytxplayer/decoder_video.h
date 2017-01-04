//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_DECODER_VIDEO_H
#define YTXPLAYER_DECODER_VIDEO_H
#include "Decoder.h"
#include "frame_queue.h"
#include <ytxplayer/VideoStateInfo.h>
typedef void (*VideoDecodingHandler) (AVFrame*,double);
typedef void (*VideoDecodeFinishHandler) ();
class DecoderVideo : public IDecoder
{
public:
    DecoderVideo(VideoStateInfo *mVideoStateInfo);
    ~DecoderVideo();

    VideoDecodingHandler		onDecode;
    VideoDecodeFinishHandler    onDecodeFinish;
    int isFinish;

    //VideoStateInfo *mVideoStateInfo;
    int frameQueueInitFinsh=0;
    struct SwsContext*	mConvertCtx;
    unsigned char *out_buffer_video;
    void stop();
private:
    AVFrame*					mFrame;
    double						mVideoClock;
    AVRational timeBase ;
    AVRational frameRate;



    bool                        prepare();
    double 						synchronize(AVFrame *src_frame, double pts);
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);
    static int					getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags);
    static void					releaseBuffer(struct AVCodecContext *c, AVFrame *pic);




};
#endif //YTXPLAYER_DECODER_VIDEO_H
