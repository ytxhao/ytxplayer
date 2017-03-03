//
// Created by Administrator on 2016/9/22.
//

#include <android/log.h>
#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/ffmsg.h>
#include "ytxplayer/decoder_subtitle.h"
#define TAG "FFMpegSubtitleDecoder"
#include "ytxplayer/ALog-priv.h"
#include "ytxplayer/frame_queue_subtitle.h"
#include "ytxplayer/test.h"
//ffplay -vf subtitles=test_file/x7_11.srt test_file/x7_11.mkv
DecoderSubtitle::DecoderSubtitle(VideoStateInfo *mVideoStateInfo):IDecoder(mVideoStateInfo)
{
   // isFirstFrame = true;
    firstInit = false;
    sprintf(subfile,"%s/test.ass",mVideoStateInfo->mStorageDir);
    //fp_pcm = fopen(subfile,"wb+");

}

DecoderSubtitle::~DecoderSubtitle()
{
    // Free audio samples buffer


    ass_renderer_done(ass_renderer);
    ass_library_done(ass_library);
    ass_free_track(track);

    if(mFrame != NULL){
        av_frame_free(&mFrame);
    }
    if(!mConvertCtx){
        sws_freeContext(mConvertCtx);
    }

    //avcodec_close(mVideoStateInfo->streamSubtitle->dec_ctx);
}

bool DecoderSubtitle::prepare()
{

    ALOGI("DecoderSubtitle::prepare\n");
    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return false;
    }


    printFontProviders(ass_library);
    init(mVideoStateInfo->mVideoWidth, mVideoStateInfo->mVideoHeight);
    track = ass_new_track(ass_library);
    if (!track) {
        ALOGI("track init failed!\n");
        return 1;
    }

    if (mVideoStateInfo->streamSubtitle->dec_ctx != NULL && mVideoStateInfo->streamSubtitle->dec_ctx->subtitle_header){
        ass_process_codec_private(track,
                                  (char *) mVideoStateInfo->streamSubtitle->dec_ctx->subtitle_header,
                                  mVideoStateInfo->streamSubtitle->dec_ctx->subtitle_header_size);
    }

 //   track = ass_read_file(ass_library,subfile , NULL);
//    char *argv[]={"ass_test","/storage/emulated/0/ass1.png","/storage/emulated/0/test.ass","25"};
//    main_test(4,argv);
    return true;
}

bool DecoderSubtitle::process(MAVPacket *mPacket)
{
    double pts = 0;
    int size = mSamplesSize;
    int	completed;
    curStats = mPacket->isEnd;
    if(curStats != lastStats && curStats && mPacket->pkt.data == NULL){
        lastStats = curStats;
        return true;
    }
    lastStats = curStats;

    if(mQueue->size() == 0){
        pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    }

    if(mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data){
        avcodec_flush_buffers(mVideoStateInfo->streamSubtitle->dec_ctx);
        return true;
    }

    if(mVideoStateInfo->pkt_serial_subtitle != mQueue->serial){
        return true;
    }


    if(mPacket->pkt.size == 0 && mPacket->pkt.data == NULL){
        return true;
    }

    if (!(sp = mVideoStateInfo->frameQueueSubtitle->frameQueuePeekWritable())) {
        return true;
    }



    int ret = avcodec_decode_subtitle2(mVideoStateInfo->streamSubtitle->dec_ctx, &sp->sub, &completed, &mPacket->pkt);

    if(completed > 0 ){

        ALOGI("sp-sub-format=%d",sp->sub.format);
    }
    if (completed > 0  && sp->sub.format == 0) {
        if (sp->sub.pts != AV_NOPTS_VALUE) {
            pts = sp->sub.pts / (double) AV_TIME_BASE;
        }
        sp->pts = pts;
        sp->serial = mVideoStateInfo->pkt_serial_subtitle;

        if (!(sp->subrects = (AVSubtitleRect **) av_mallocz_array(sp->sub.num_rects, sizeof(AVSubtitleRect*)))) {
            ALOGI("Cannot allocate subrects\n");
            exit(1);
        }

        for (int i = 0; i < sp->sub.num_rects; i++)
        {
            int in_w = sp->sub.rects[i]->w;
            int in_h = sp->sub.rects[i]->h;
            int subw = mVideoStateInfo->streamSubtitle->dec_ctx->width  ? mVideoStateInfo->streamSubtitle->dec_ctx->width  : mVideoStateInfo->mVideoWidth;
            int subh = mVideoStateInfo->streamSubtitle->dec_ctx->height ? mVideoStateInfo->streamSubtitle->dec_ctx->height : mVideoStateInfo->mVideoHeight;
            int out_w = mVideoStateInfo->mVideoWidth  ? in_w * mVideoStateInfo->mVideoWidth  / subw : in_w;
            int out_h = mVideoStateInfo->mVideoHeight ? in_h * mVideoStateInfo->mVideoHeight / subh : in_h;

            if (!(sp->subrects[i] = (AVSubtitleRect *) av_mallocz(sizeof(AVSubtitleRect))) ||
                av_image_alloc(sp->subrects[i]->data, sp->subrects[i]->linesize, out_w, out_h, AV_PIX_FMT_YUVA420P, 16) < 0) {
                ALOGI("Cannot allocate subtitle data\n");
                exit(1);
            }

            mConvertCtx = sws_getCachedContext(mConvertCtx,
                                                       in_w, in_h, AV_PIX_FMT_PAL8, out_w, out_h,
                                                       AV_PIX_FMT_YUVA420P, sws_flags, NULL, NULL, NULL);
            if (!mConvertCtx) {
                ALOGI("Cannot initialize the sub conversion context\n");
                exit(1);
            }
            sws_scale(mConvertCtx,
                      (const uint8_t *const *) sp->sub.rects[i]->data, sp->sub.rects[i]->linesize,
                      0, in_h, sp->subrects[i]->data, sp->subrects[i]->linesize);

            sp->subrects[i]->w = out_w;
            sp->subrects[i]->h = out_h;
            sp->subrects[i]->x = sp->sub.rects[i]->x * out_w / in_w;
            sp->subrects[i]->y = sp->sub.rects[i]->y * out_h / in_h;


        }
        /* now we can update the picture count */
        mVideoStateInfo->frameQueueSubtitle->frameQueuePush();


    }else if(completed > 0 && sp->sub.format == 1){

        if (sp->sub.pts != AV_NOPTS_VALUE) {
            pts = sp->sub.pts / (double) AV_TIME_BASE;
        }
        sp->pts = pts;
        sp->serial = mVideoStateInfo->pkt_serial_subtitle;

        const int64_t start_time = av_rescale_q(sp->sub.pts, AV_TIME_BASE_Q, av_make_q(1, 1000));
        const int64_t duration   = sp->sub.end_display_time;

        for (int i = 0; i < sp->sub.num_rects; i++){
            ALOGI("ttttt1 sp->sub.rects[%d]->type=%d, sp->sub.rects[%d]->ass=%s\n",i,sp->sub.rects[i]->type,i,sp->sub.rects[i]->ass);



            char *ass_line = sp->sub.rects[i]->ass;
            if (!ass_line){
                break;
            }
//            if (LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57,25,100)){
//                ass_process_data(track,ass_line, strlen(ass_line));
//            } else {
//                ass_process_chunk(track, ass_line, strlen(ass_line),
//                                  start_time, duration);
//            }


            ass_process_data(track, ass_line, strlen(ass_line));

            track->events->Start = start_time;
            ASS_Image *img = ass_render_frame(ass_renderer, track, start_time, NULL);
            sp->imageFrame = gen_image(mVideoStateInfo->mVideoWidth, mVideoStateInfo->mVideoHeight);
            blend(sp->imageFrame, img);
//            x++;
//            sprintf(subpng,"%s/test%d.png",mVideoStateInfo->mStorageDir,x);
//            write_png(subpng, sp->imageFrame);
//            free(sp->imageFrame->buffer);
//            free(sp->imageFrame);
            ass_flush_events(track);
            mVideoStateInfo->frameQueueSubtitle->frameQueuePush();
        }


    }else if(completed){
         avsubtitle_free(&sp->sub);
    }

    return true;
}

bool DecoderSubtitle::decode(void* ptr)
{
    int i;
    MAVPacket        pPacket;

    while(mRunning)
    {
        if(mQueue->get(&pPacket, true,&mVideoStateInfo->pkt_serial_audio) < 0)
        {
            mRunning = false;
            return false;
        }
        if(!process(&pPacket))
        {
            mRunning = false;
            return false;
        }
        // Free the packet that was allocated by av_read_frame
        av_packet_unref(&pPacket.pkt);
    }

    return true;
}

void DecoderSubtitle::stop() {
    mRunning = false;
    mQueue->abort();
    mVideoStateInfo->frameQueueAudio->frameQueueReset();
    ALOGI("waiting on end of decoder thread\n");
    int ret = -1;
    if((ret = wait()) != 0) {
        ALOGI("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}

int DecoderSubtitle::streamHasEnoughPackets(){
    int ret = 0;
    ret = mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] < 0 ||
          mQueue->mAbortRequest ||
          (mVideoStateInfo->streamAudio->st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
          mQueue->size() > MIN_FRAMES && (!mQueue->duration || av_q2d(mVideoStateInfo->streamAudio->st->time_base) * mQueue->duration > 1.0);

    return ret;
}

void DecoderSubtitle::printFontProviders(ASS_Library *ass_library) {
    int i;
    ASS_DefaultFontProvider *providers;
    size_t providers_size = 0;
    ass_get_available_font_providers(ass_library, &providers, &providers_size);
    ALOGI("test.c: Available font providers (%zu): ", providers_size);
    for (i = 0; i < providers_size; i++) {
        const char *separator = i > 0 ? ", ": "";
        ALOGI("%s'%s'", separator,  font_provider_labels[providers[i]]);
    }
    ALOGI(".\n");
    free(providers);
}

void DecoderSubtitle::init(int frame_w, int frame_h) {
    ass_library = ass_library_init();
    if (!ass_library) {
        ALOGI("ass_library_init failed!\n");
        exit(1);
    }

    ass_set_message_cb(ass_library, msg_callback, NULL);

    ass_renderer = ass_renderer_init(ass_library);
    if (!ass_renderer) {
        ALOGI("ass_renderer_init failed!\n");
        exit(1);
    }

    ass_set_frame_size(ass_renderer, frame_w, frame_h);
//    ass_set_fonts(ass_renderer, "/storage/emulated/0/tang_cn.ttf", "sans-serif",
//                  ASS_FONTPROVIDER_AUTODETECT, NULL, 1);
    ass_set_fonts(ass_renderer, mVideoStateInfo->join3(mVideoStateInfo->mStorageDir,"tang_cn.ttf"), "sans-serif",
                  ASS_FONTPROVIDER_AUTODETECT, NULL, 1);
}


void DecoderSubtitle::msg_callback(int level, const char *fmt, va_list va, void *data) {
    if (level > 6)
        return;
    ALOGI("libass: ");
    vprintf(fmt, va);
    ALOGI("\n");
}

image_t* DecoderSubtitle::gen_image(int width, int height) {

    image_t *img = (image_t *) malloc(sizeof(image_t));
    img->width = width;
    img->height = height;
    img->stride = width * 3;
    img->buffer = (unsigned char *) calloc(1, height * width * 3);
    memset(img->buffer, 0, img->stride * img->height);
    //for (int i = 0; i < height * width * 3; ++i)
    // img->buffer[i] = (i/3/50) % 100;
    return img;
}

void DecoderSubtitle::blend(image_t *frame, ASS_Image *img) {
    int cnt = 0;
    while (img) {
        blend_single(frame, img);
        ++cnt;
        img = img->next;
    }
    ALOGI("%d images blended\n", cnt);
}

void DecoderSubtitle::blend_single(image_t *frame, ASS_Image *img) {
    int x, y;
    unsigned char opacity = 255 - _a(img->color);
    unsigned char r = _r(img->color);
    unsigned char g = _g(img->color);
    unsigned char b = _b(img->color);

    unsigned char *src;
    unsigned char *dst;
    src = img->bitmap;
    dst = frame->buffer + img->dst_y * frame->stride + img->dst_x * 3;
    for (y = 0; y < img->h; ++y) {
        for (x = 0; x < img->w; ++x) {
            unsigned k = ((unsigned) src[x]) * opacity / 255;
            // possible endianness problems
            dst[x * 3] = (k * b + (255 - k) * dst[x * 3]) / 255;
            dst[x * 3 + 1] = (k * g + (255 - k) * dst[x * 3 + 1]) / 255;
            dst[x * 3 + 2] = (k * r + (255 - k) * dst[x * 3 + 2]) / 255;
        }
        src += img->stride;
        dst += frame->stride;
    }
}


void DecoderSubtitle::write_png(char *fname, image_t *img) {

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte **row_pointers;
    int k;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    fp = NULL;

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    fp = fopen(fname, "wb");
    if (fp == NULL) {
        printf("PNG Error opening %s for writing!\n", fname);
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, 0);

    png_set_IHDR(png_ptr, info_ptr, img->width, img->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_set_bgr(png_ptr);

    row_pointers = (png_byte **) malloc(img->height * sizeof(png_byte *));
    for (k = 0; k < img->height; k++)
        row_pointers[k] = img->buffer + img->stride * k;

    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(row_pointers);

    fclose(fp);
}