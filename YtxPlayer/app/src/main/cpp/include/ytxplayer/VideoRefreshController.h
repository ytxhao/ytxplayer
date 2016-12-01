//
// Created by Administrator on 2016/11/30.
//

#ifndef YTXPLAYER_VIDEOREFRESHTHREAD_H
#define YTXPLAYER_VIDEOREFRESHTHREAD_H
#include <ytxplayer/decoder_video.h>
#include "frame_queue.h"
#include "Thread.h"
class VideoRefreshController : public Thread
{
public:
    double last_duration, duration, delay;
    Frame *vp, *lastvp;
    double remaining_time = 0.0;
    double time;
    double frame_timer=0.0;
    DecoderVideo*  mDecoderVideo;
    VideoRefreshController(DecoderVideo* mDecoderVideo);
    ~VideoRefreshController(){};

    void				handleRun(void* ptr);
    bool                prepare();
    void                refresh();
    void                process();
    double              vp_duration(Frame *vp, Frame *next_vp);

};


#endif //YTXPLAYER_VIDEOREFRESHTHREAD_H
