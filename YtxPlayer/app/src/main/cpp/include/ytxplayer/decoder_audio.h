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
    int streamHasEnoughPackets();
    struct SwrContext*	mConvertCtx = NULL;
private:


    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);
    bool                        audioOpen(int64_t wanted_channel_layout, int wanted_nb_channels,
                                          int wanted_sample_rate,
                                          struct AudioParams *audio_hw_params);

    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;

    AVRational tb;
    Frame *af;

    int64_t next_pts;
    AVRational next_pts_tb;
    AVMessage msg;

    bool lastStats;
    bool curStats;

    bool firstInit;
 //   bool                        isFirstAudioFrame;
};

#endif //YTXPLAYER_DECODER_AUDIO_H
