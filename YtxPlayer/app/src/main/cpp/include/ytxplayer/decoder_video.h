//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_DECODER_VIDEO_H
#define YTXPLAYER_DECODER_VIDEO_H
#include "Decoder.h"
#include "frame_queue.h"
#include "ytxplayer/ffinc.h"
#include <ytxplayer/VideoStateInfo.h>
typedef void (*VideoDecodingHandler) (AVFrame*,double);

class DecoderVideo : public IDecoder
{
public:
    DecoderVideo(VideoStateInfo *mVideoStateInfo);
    ~DecoderVideo();

    VideoDecodingHandler		onDecode;

    int isFinish;

    int frameQueueInitFinsh=0;
    struct SwsContext*	mConvertCtx = NULL;
    unsigned char *out_buffer_video;
    void stop();
    int                         streamHasEnoughPackets();
private:
    AVFrame*					mFrame;
    AVFrame*                    mFrameYuv;
    double						mVideoClock;
    AVRational timeBase ;
    AVRational frameRate;
    AVFilterGraph *graph;
    AVFilterContext *filt_out = NULL, *filt_in = NULL;
    int last_w = 0;
    int last_h = 0;
    enum AVPixelFormat last_format = AV_PIX_FMT_NONE;
    int last_serial = -1;
    int last_vfilter_idx = 0;



    bool                        prepare();
    double 						synchronize(AVFrame *src_frame, double pts);
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);
    char file[100];
    static int					getBuffer(struct AVCodecContext *c, AVFrame *pic,int flags);
    static void					releaseBuffer(struct AVCodecContext *c, AVFrame *pic);
    bool lastStats;
    bool curStats;
    bool firstInit ;

    int configure_video_filters(AVFilterGraph *graph, const char *vfilters, AVFrame *frame);
    int configure_filtergraph(AVFilterGraph *graph, const char *filtergraph,
                              AVFilterContext *source_ctx, AVFilterContext *sink_ctx);



};
#endif //YTXPLAYER_DECODER_VIDEO_H
