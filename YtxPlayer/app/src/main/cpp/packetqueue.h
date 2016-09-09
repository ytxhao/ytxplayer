//
// Created by Administrator on 2016/9/7.
//

#ifndef YTXPLAYER_PACKETQUEUE_H
#define YTXPLAYER_PACKETQUEUE_H



#include <pthread.h>

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

typedef struct MAVPacketList {
    int i;
    AVPacket pkt;
    struct MAVPacketList *next;
} MAVPacketList;

class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();

    void flush();
    int put(AVPacket* pkt,int *i);

    /* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
    int get(AVPacket *pkt, bool block,int *i);

    int size();

    void abort();

private:
    MAVPacketList*		mFirst;
    MAVPacketList*		mLast;
    int					mNbPackets;
    int					mSize;
    bool				mAbortRequest;
    pthread_mutex_t     mLock;
    pthread_cond_t		mCondition;
};




#endif //YTXPLAYER_PACKETQUEUE_H
