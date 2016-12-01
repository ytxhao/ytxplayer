//
// Created by Administrator on 2016/9/7.
//

#define TAG "FFMpegPacketQueue"

#include "ytxplayer/packetqueue.h"

PacketQueue::PacketQueue()
{
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
    mFirst = NULL;
    mLast = NULL;
    mNbPackets = 0;;
    mSize = 0;
    mAbortRequest = false;
}

PacketQueue::~PacketQueue()
{
    flush();
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

int PacketQueue::size()
{
    pthread_mutex_lock(&mLock);
    int size = mNbPackets;
    pthread_mutex_unlock(&mLock);
    return size;
}

void PacketQueue::flush()
{
    MAVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&mLock);

    for(pkt = mFirst; pkt != NULL; pkt = pkt1) {
        av_free_packet(&pkt->mPkt.pkt);
        av_freep(&pkt);
        pkt1 = pkt->next;
    }
    mLast = NULL;
    mFirst = NULL;
    mNbPackets = 0;
    mSize = 0;

    pthread_mutex_unlock(&mLock);
}

int PacketQueue::put(MAVPacket* mPkt)
{
    MAVPacketList *pkt1;

    /* duplicate the packet */
    if (av_dup_packet(&mPkt->pkt) < 0)
        return -1;

    pkt1 = (MAVPacketList *) av_malloc(sizeof(MAVPacketList));
    if (!pkt1)
        return -1;
    pkt1->mPkt = *mPkt;
    pkt1->next = NULL;

    pthread_mutex_lock(&mLock);

    if (!mLast) {
        mFirst = pkt1;
    } else {
        mLast->next = pkt1;
    }

    mLast = pkt1;
    mNbPackets++;
    mSize += pkt1->mPkt.pkt.size + sizeof(*pkt1);

    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);

    return 0;

}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int PacketQueue::get(MAVPacket *mPkt, bool block)
{
    MAVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&mLock);

    for(;;) {
        if (mAbortRequest) {
            ret = -1;
            break;
        }

        pkt1 = mFirst;
        if (pkt1) {
            mFirst = pkt1->next;
            if (!mFirst)
                mLast = NULL;
            mNbPackets--;
            mSize -= pkt1->mPkt.pkt.size + sizeof(*pkt1);
            *mPkt = pkt1->mPkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&mCondition, &mLock);
        }

    }
    pthread_mutex_unlock(&mLock);
    return ret;

}

void PacketQueue::abort()
{
    pthread_mutex_lock(&mLock);
    mAbortRequest = true;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}
