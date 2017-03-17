//
// Created by Administrator on 2016/12/6.
//

#ifndef YTXPLAYER_FFMSG_H
#define YTXPLAYER_FFMSG_H

#define FFP_MSG_FLUSH                       0
#define FFP_MSG_ERROR                       100     /* arg1 = error */
#define FFP_MSG_PREPARED                    200
#define FFP_MSG_COMPLETED                   300
#define FFP_MSG_VIDEO_SIZE_CHANGED          400     /* arg1 = width, arg2 = height */
#define FFP_MSG_SAR_CHANGED                 401     /* arg1 = sar.num, arg2 = sar.den */
#define FFP_MSG_VIDEO_RENDERING_START       402
#define FFP_MSG_AUDIO_RENDERING_START       403
#define FFP_MSG_VIDEO_ROTATION_CHANGED      404     /* arg1 = degree */
#define FFP_MSG_BUFFERING_START             500
#define FFP_MSG_BUFFERING_END               501
#define FFP_MSG_BUFFERING_UPDATE            502     /* arg1 = buffering head position in time, arg2 = minimum percent in time or bytes */
#define FFP_MSG_BUFFERING_BYTES_UPDATE      503     /* arg1 = cached data in bytes,            arg2 = high water mark */
#define FFP_MSG_BUFFERING_TIME_UPDATE       504     /* arg1 = cached duration in milliseconds, arg2 = high water mark */
#define FFP_MSG_SEEK_COMPLETE               600     /* arg1 = seek position,                   arg2 = error */
#define FFP_MSG_PLAYBACK_STATE_CHANGED      700

#define FFP_MSG_VIDEO_DECODER_OPEN          10001

#define FFP_MSG_CANCEL                      10002


#define FFP_MSG_AUDIO_FIRST_FRAME           20001


enum media_event_type {
    MEDIA_NOP               = 0,        // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,        // arg1 = percentage, arg2 = cached duration
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,        // arg1 = width, arg2 = height
    MEDIA_TIMED_TEXT        = 99,       // not supported yet
    MEDIA_ERROR             = 100,      // arg1, arg2
    MEDIA_INFO              = 200,      // arg1, arg2

    MEDIA_STOPPED           = 201,


    MEDIA_SET_VIDEO_SAR     = 10001,    // arg1 = sar.num, arg2 = sar.den
};


enum media_error_type {
    MEDIA_ERROR_OPEN_STREAM               = 0,        //Couldn't open input stream

};

#endif //YTXPLAYER_FFMSG_H
