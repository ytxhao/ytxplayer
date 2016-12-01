//
// Created by Administrator on 2016/9/22.
//

#ifndef YTXPLAYER_DECODER_AUDIO_H
#define YTXPLAYER_DECODER_AUDIO_H


#include "Decoder.h"
#include "frame_queue.h"
typedef void (*AudioDecodingHandler) (AVFrame*,double);
typedef void (*AudioDecodeFirstFrameHandler) ();

class DecoderAudio : public IDecoder
{
public:
    DecoderAudio(VideoStateInfo *mVideoStateInfo);

    ~DecoderAudio();

    AudioDecodingHandler		onDecode;
    AudioDecodeFirstFrameHandler  firstFrameHandler;
   // FrameQueue *frameQueue;
    VideoStateInfo *mVideoStateInfo;
   // void setFrameQueue(FrameQueue *frameQueue){this->frameQueue = frameQueue;}
private:
    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;
    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(AVPacket *packet,int *i);
    AVRational tb;
    Frame *af;

    int64_t next_pts;
    AVRational next_pts_tb;

    bool                        isFirstFrame;
};

#endif //YTXPLAYER_DECODER_AUDIO_H
