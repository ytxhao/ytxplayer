//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_DECODER_H
#define YTXPLAYER_DECODER_H


#ifndef FFMPEG_DECODER_H
#define FFMPEG_DECODER_H

#include "ffinc.h"

#include "thread.h"
#include "packetqueue.h"

class IDecoder : public Thread
{
public:
    IDecoder(InputStream* stream);
    ~IDecoder();

    void						stop();
    void						enqueue(AVPacket* packet);
    int							packets();

protected:
    PacketQueue*                mQueue;
  //  AVStream*             		mStream;
    InputStream*                mStream;
    virtual bool                prepare();
    virtual bool                decode(void* ptr);
    virtual bool                process(AVPacket *packet);
    void						handleRun(void* ptr);
};

#endif //FFMPEG_DECODER_H

#endif //YTXPLAYER_DECODER_H
