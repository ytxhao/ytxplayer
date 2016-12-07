//
// Created by Administrator on 2016/12/6.
//

#ifndef YTXPLAYER_MEDIAPLAYERLISTENER_H
#define YTXPLAYER_MEDIAPLAYERLISTENER_H

// ref-counted object for callbacks
class MediaPlayerListener
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
};

#endif //YTXPLAYER_MEDIAPLAYERLISTENER_H
