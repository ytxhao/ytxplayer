//
// Created by Administrator on 2016/9/22.
//

#ifndef YTXPLAYER_DECODER_SUBTITLE_H
#define YTXPLAYER_DECODER_SUBTITLE_H


#include "Decoder.h"
#include "frame_queue.h"

class DecoderSubtitle : public IDecoder
{
public:
    DecoderSubtitle(VideoStateInfo *mVideoStateInfo);

    ~DecoderSubtitle();

    void stop();
    int streamHasEnoughPackets();
private:
    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;
    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);
    AVRational tb;
    Frame *sp;

    int64_t next_pts;
    AVRational next_pts_tb;
    AVMessage msg;

    bool lastStats;
    bool curStats;

    bool firstInit;
 //   bool                        isFirstAudioFrame;
};

#endif //YTXPLAYER_DECODER_AUDIO_H
