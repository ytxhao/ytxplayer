//
// Created by Administrator on 2016/9/22.
//

#ifndef YTXPLAYER_DECODER_AUDIO_H
#define YTXPLAYER_DECODER_AUDIO_H


#include "decoder.h"

typedef void (*AudioDecodingHandler) (AVFrame*,double);

class DecoderAudio : public IDecoder
{
public:
    DecoderAudio(InputStream* stream);

    ~DecoderAudio();

    AudioDecodingHandler		onDecode;

private:
    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;
    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(AVPacket *packet,int *i);
};

#endif //YTXPLAYER_DECODER_AUDIO_H
