//
// Created by Administrator on 2016/12/9.
//

#ifndef YTXPLAYER_AVPACKETLIST_H
#define YTXPLAYER_AVPACKETLIST_H
typedef struct MAVPacket {
    AVPacket pkt;
    bool isEnd = false;
} MAVPacket;



typedef struct MAVPacketList {
    MAVPacket mPkt;
    struct MAVPacketList *next;
    int serial;
} MAVPacketList;
#endif //YTXPLAYER_AVPACKETLIST_H
