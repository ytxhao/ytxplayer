//
// Created by Administrator on 2016/9/22.
//

#ifndef YTXPLAYER_DECODER_AUDIO_H
#define YTXPLAYER_DECODER_AUDIO_H


#include "Decoder.h"
#include "frame_queue.h"
typedef void (*AudioDecodingHandler) (AVFrame*,double);
typedef void (*AudioDecodeFirstFrameHandler) ();
typedef void (*AudioDecodeFlushFrameHandler) ();

class DecoderAudio : public IDecoder
{
public:
    DecoderAudio(VideoStateInfo *mVideoStateInfo);

    ~DecoderAudio();

    AudioDecodingHandler		onDecode;
    AudioDecodeFirstFrameHandler  firstFrameHandler;
   // AudioDecodeFlushFrameHandler  flushFrameHandler;
    //VideoStateInfo *mVideoStateInfo;

    void stop();
private:
    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;
    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);
    AVRational tb;
    Frame *af;

    int64_t next_pts;
    AVRational next_pts_tb;
    AVMessage msg;

 //   bool                        isFirstAudioFrame;
};

#endif //YTXPLAYER_DECODER_AUDIO_H
