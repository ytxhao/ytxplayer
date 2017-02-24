//
// Created by Administrator on 2016/12/1.
//
#define NDEBUG 0
#define TAG "VideoStateInfo-jni"


#include <ytxplayer/android_media_YtxMediaPlayer.h>
#include "ytxplayer/VideoStateInfo.h"
#include "ytxplayer/ALog-priv.h"
VideoStateInfo::VideoStateInfo(){
    frameQueueVideo = new FrameQueueVideo();
    frameQueueAudio = new FrameQueueAudio();
    frameQueueSubtitle = new FrameQueueSubtitle();
    streamVideo = new InputStream();
    streamAudio = new InputStream();
    streamSubtitle = new InputStream();
    messageQueueAudio = new MessageQueue();
    messageQueueVideo = new MessageQueue();
    mMessageLoop = new MessageLoop();
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
    pthread_mutex_init(&wait_mutex,NULL);
    pthread_cond_init(&continue_read_thread,NULL);
    seekReq = false;
    flushPkt = (MAVPacket *)malloc(sizeof(MAVPacket));
    av_init_packet(&flushPkt->pkt);
    flushPkt->pkt.data = (uint8_t *)&flushPkt->pkt;
    ALOGI("VideoStateInfo flushPkt->pkt.data= %#x\n",flushPkt->pkt.data);
    vidClk = (Clock *)malloc(sizeof(Clock));
    extClk = (Clock *)malloc(sizeof(Clock));
    audClk = (Clock *)malloc(sizeof(Clock));
    memset(st_index, -1, sizeof(st_index));

    max_frame_duration = 0.0;
    seekPos = 0;
    seekRel = 0;
    seekFlags = 0;
    eof =0;
    currentTime =0;
//    sprintf(file_pcm,"/storage/emulated/0/ytx.pcm");
//    fp_pcm = fopen(file_pcm,"wb+");
//    sprintf(file_pcm1,"/storage/emulated/0/ytx1.pcm");
//    fp_pcm1 = fopen(file_pcm1,"wb+");

}
VideoStateInfo::~VideoStateInfo() {
//    fclose(fp_pcm);
//    fclose(fp_pcm1);
    avformat_close_input(&pFormatCtx);
    delete frameQueueVideo;
    delete frameQueueAudio;
    delete frameQueueSubtitle;
    delete streamSubtitle;
    delete streamVideo;
    delete streamAudio;
    delete messageQueueAudio;
    delete messageQueueVideo;
    delete mMessageLoop;

//    JNIEnv *env = getJNIEnv();
//    env->DeleteGlobalRef(VideoGlSurfaceViewObj);
//    env->DeleteGlobalRef(GraphicRendererObj);

    free(flushPkt);
    free(vidClk);
    free(extClk);
    free(audClk);
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
    pthread_mutex_destroy(&wait_mutex);
    pthread_cond_destroy(&continue_read_thread);
}

void VideoStateInfo::notify() {
    pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}


void VideoStateInfo::notifyAll() {
    pthread_mutex_lock(&mLock);
    pthread_cond_broadcast(&mCondition);
    pthread_mutex_unlock(&mLock);
}

void VideoStateInfo::waitOnNotify(int mCurrentState)
{
    pthread_mutex_lock(&mLock);
    while (*this->mCurrentState == mCurrentState){
        pthread_cond_wait(&mCondition, &mLock);
    }

    pthread_mutex_unlock(&mLock);
}


double VideoStateInfo::getClock(Clock *c) {
    if (*c->queue_serial != c->serial)
        return NAN;

    if (c->paused) {
        return c->pts;
    } else {
        double time = av_gettime_relative() / 1000000.0;
        return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
    }

}
void VideoStateInfo::setClock(Clock *c, double pts, int serial) {

    double time = av_gettime_relative() / 1000000.0;
    setClockAt(c, pts, serial, time);

}

void VideoStateInfo::setClockAt(Clock *c, double pts, int serial, double time) {
    c->pts = pts;
    c->last_updated = time;
    c->pts_drift = c->pts - time;
    c->serial = serial;
}
void VideoStateInfo::setClockSpeed(Clock *c, double speed) {

    setClock(c, getClock(c), c->serial);
    c->speed = speed;
}

void VideoStateInfo::initClock(Clock *c, int *queue_serial) {
    c->speed = 1.0;
    c->paused = 0;
    c->queue_serial = queue_serial;
    setClock(c, NAN, -1);
}


void VideoStateInfo::updateVideoPts(double pts, int64_t pos, int serial) {
    /* update current video pts */
    setClock(vidClk, pts, serial);
    syncClock2Slave(extClk, vidClk);
}

void VideoStateInfo::syncClock2Slave(Clock *c, Clock *slave) {
    double clock = getClock(c);
    double slave_clock = getClock(slave);
    if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
        setClock(c, slave_clock, slave->serial);
}


char* VideoStateInfo::join3(char *s1, char *s2)
{
    char *result = (char *) malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}