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

#include "avPacketList.h"
#include "VideoStateInfo.h"

class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();

    void flush();
    int put(MAVPacket* pkt);

    /* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
    int get(MAVPacket *pkt, bool block);

    int size();

    void abort();
    int serial;
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
