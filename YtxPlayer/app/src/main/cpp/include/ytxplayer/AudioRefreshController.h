//
// Created by Administrator on 2016/11/30.
//

#ifndef YTXPLAYER_AUDIOREFRESHTHREAD_H
#define YTXPLAYER_AUDIOREFRESHTHREAD_H
#include <ytxplayer/decoder_video.h>
#include "frame_queue.h"
#include "Thread.h"
#include "MessageQueue.h"
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/audio_engine.h>

class AudioRefreshController : public Thread
{
public:
    double last_duration, duration, delay;
    //Frame *vp, *lastvp;
    double remaining_time = 0.0;
    double time;
    double frame_timer=0.0;
    int64_t audio_callback_time;
    VideoStateInfo* mVideoStateInfo = NULL;
    AudioEngine *mAudioEngine = NULL;

    AudioRefreshController(VideoStateInfo* mVideoStateInfo);
    ~AudioRefreshController();

    void				handleRun(void* ptr);
    bool                prepare();
    void                refresh();
    bool                process(AVMessage *msg);
    void                stop();
    static void         bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
    int                 audioFrameProcess();
    Frame               *audioDecodeFrame();
    void                enqueue(AVMessage *msg);
};


#endif //YTXPLAYER_AUDIOREFRESHTHREAD_H
