//
// Created by Administrator on 2016/12/1.
//
#include "VideoStateInfo.h"

VideoStateInfo::VideoStateInfo(){
    frameQueueVideo = new FrameQueue();
    frameQueueAudio = new FrameQueue();
    streamVideo = new InputStream();
    streamAudio = new InputStream();

}

VideoStateInfo::~VideoStateInfo() {
    delete frameQueueVideo;
    delete frameQueueAudio;
    delete streamVideo;
    delete streamAudio;
}