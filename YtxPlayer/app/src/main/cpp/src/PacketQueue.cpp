//
// Created by Administrator on 2016/9/7.
//

#define TAG "FFMpegPacketQueue"

#include "ALog-priv.h"
#include "ytxplayer/packetqueue.h"

PacketQueue::PacketQueue(VideoStateInfo *mVideoStateInfo)
{
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
    mFirst = NULL;
    mLast = NULL;
    mNbPackets = 0;;
    mSize = 0;
    mAbortRequest = false;
    this->mVideoStateInfo = mVideoStateInfo;
    this->serial = 0;
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
        pkt1 = pkt->next;
        av_packet_unref(&pkt->mPkt.pkt);
        av_freep(&pkt);
    }
    mLast = NULL;
    mFirst = NULL;
    mNbPackets = 0;
    mSize = 0;

    pthread_mutex_unlock(&mLock);
}

int PacketQueue::putNullPacket(int stream_index) {
    MAVPacket pkt1, *pkt = &pkt1;
    av_init_packet(&pkt->pkt);
    pkt->isEnd = true;
    pkt->pkt.data = NULL;
    pkt->pkt.size = 0;
    pkt->pkt.stream_index = stream_index;
    return put(pkt);
}

int PacketQueue::put(MAVPacket* mPkt)
{

    MAVPacketList *pkt1;
    pthread_mutex_lock(&mLock);
//    if(mPkt == mVideoStateInfo->flushPkt){
//        ALOGI("PacketQueue0 mVideoStateInfo->flushPkt->pkt.data=%#x mPkt->pkt.data=%#x\n",mVideoStateInfo->flushPkt->pkt.data,mPkt->pkt.data);
//
//    }
//    /* duplicate the packet */
//    if (av_dup_packet(&mPkt->pkt) < 0)
//        return -1;

    pkt1 = (MAVPacketList *) av_malloc(sizeof(MAVPacketList));
    if (!pkt1)
        return -1;
    pkt1->mPkt = *mPkt;
    pkt1->next = NULL;

    if(mPkt == mVideoStateInfo->flushPkt){
        ALOGI("PacketQueue1 mVideoStateInfo->flushPkt->pkt.data=%#x pkt1->mPkt.pkt.data=%#x mPkt->pkt.data=%#x\n",mVideoStateInfo->flushPkt->pkt.data,pkt1->mPkt.pkt.data,mPkt->pkt.data);
        this->serial++;  //如果当前加入的是flush包,则包队列的序列号加一
    }
    pkt1->serial = this->serial;//将入队列的包序列号赋值为该包队列的序列号



    if (!mLast) {
        mFirst = pkt1;
    } else {
        mLast->next = pkt1;
    }

    mLast = pkt1;
    mNbPackets++;
    mSize += pkt1->mPkt.pkt.size + sizeof(*pkt1);
    duration += pkt1->mPkt.pkt.duration;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);

    return 0;

}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int PacketQueue::get(MAVPacket *mPkt, bool block,int *serial)
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
            duration -= pkt1->mPkt.pkt.duration;
            *mPkt = pkt1->mPkt;
            if (serial) {
                *serial = pkt1->serial;
            }
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
