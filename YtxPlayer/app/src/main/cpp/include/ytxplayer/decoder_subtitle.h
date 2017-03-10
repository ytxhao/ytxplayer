//
// Created by Administrator on 2016/9/22.
//

#ifndef YTXPLAYER_DECODER_SUBTITLE_H
#define YTXPLAYER_DECODER_SUBTITLE_H


#include "Decoder.h"
#include "frame_queue.h"
#include "ass/ass.h"
#include <png.h>



#define _r(c)  ((c)>>24)
#define _g(c)  (((c)>>16)&0xFF)
#define _b(c)  (((c)>>8)&0xFF)
#define _a(c)  ((c)&0xFF)

class DecoderSubtitle : public IDecoder
{
public:
    DecoderSubtitle(VideoStateInfo *mVideoStateInfo);

    ~DecoderSubtitle();

    void stop();
    int streamHasEnoughPackets();
    struct SwsContext *mConvertCtx = NULL;
    void printFontProviders(ASS_Library *ass_library);
    void init(int frame_w, int frame_h);
    static void msg_callback(int level, const char *fmt, va_list va, void *data);
    image_t *gen_image(int width, int height);
    void blend(image_t * frame, ASS_Image *img);
    void blend_single(image_t * frame, ASS_Image *img);
    void write_png(char *fname, image_t *img);
private:

    bool                        prepare();
    bool                        decode(void* ptr);
    bool                        process(MAVPacket *mPacket);

    int16_t*                    mSamples;
    int                         mSamplesSize;
    AVFrame*					mFrame;
    AVRational tb;
    Frame *sp;

    int64_t next_pts;
    AVRational next_pts_tb;
    AVMessage msg;

    bool lastStats;
    bool curStats;

    bool firstInit;

    ASS_Library *ass_library;
    ASS_Renderer *ass_renderer;
    ASS_Track *track;

    char *font_provider_labels[5] = {
            [ASS_FONTPROVIDER_NONE]       = "None",
            [ASS_FONTPROVIDER_AUTODETECT] = "Autodetect",
            [ASS_FONTPROVIDER_CORETEXT]   = "CoreText",
            [ASS_FONTPROVIDER_FONTCONFIG] = "Fontconfig",
            [ASS_FONTPROVIDER_DIRECTWRITE]= "DirectWrite",
    };
    //char subfile[100];
   // long long now=2500;

};

#endif //YTXPLAYER_DECODER_AUDIO_H
