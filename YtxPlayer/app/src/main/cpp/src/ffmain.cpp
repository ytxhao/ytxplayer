//
// Created by Administrator on 2016/9/8.
//
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
};
#endif

#include <fstream>
typedef struct InputStream {
    AVStream *st;

    AVCodecContext *dec_ctx;
} InputStream;

int mVideoWidth;
int mVideoHeight;
int mAudioSessionId;
float mSendLevel;
//  struct sockaddr_in          mRetransmitEndpoint;
bool mRetransmitEndpointValid;
const char *filePath;
AVFormatContext *pFormatCtx;
int mAudioStreamIndex;
int mVideoStreamIndex;
const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = {0};
int  mDuration;
struct SwsContext*	mConvertCtx;
AVFrame*					mFrame;
pthread_mutex_t             mLock;

int mStreamType;
int mCurrentState;
int st_index[AVMEDIA_TYPE_NB];
InputStream streamVideo;

int main()
{


	AVPacket *packet;
    filePath = "Titanic.mkv";
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    printf("prepare this->filePath=%s\n",filePath);
    //   ALOGI("Couldn't open input stream.\n");
    if(avformat_open_input(&pFormatCtx,filePath,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }

    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }


	
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	
/*
    for(int i=0; i<pFormatCtx->nb_streams; i++) {
        AVStream *st = pFormatCtx->streams[i];
        enum AVMediaType type = st->codecpar->codec_type;
        st->discard = AVDISCARD_ALL;
        if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1) {
            if (avformat_match_stream_specifier(pFormatCtx, st, wanted_stream_spec[type]) > 0) {
                st_index[type] = i;
            }
        }

    }

    for (int i = 0; i < AVMEDIA_TYPE_NB; i++) {
        if (wanted_stream_spec[i] && st_index[i] == -1) {
            printf("Stream specifier %s does not match any %s stream\n", wanted_stream_spec[(AVMediaType)i], av_get_media_type_string((AVMediaType)i));
            st_index[i] = INT_MAX;
        }
    }

    mVideoStreamIndex = -1;
    for(int i=0; i<pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            mVideoStreamIndex = i;
            break;
        }

//        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            mAudioStreamIndex = i;
//            break;
//        }

    }
    printf("mVideoStreamIndex=%d",mVideoStreamIndex);

    mAudioStreamIndex = -1;
    for(int i=0; i<pFormatCtx->nb_streams; i++) {

        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            mAudioStreamIndex = i;
            break;
        }

    }
    printf("mAudioStreamIndex=%d",mAudioStreamIndex);
//
//    if(mAudioStreamIndex == -1){
//        ALOGI("Didn't find a audio stream.\n");
//        return -1;
//    }
//
//
//    if(mVideoStreamIndex==-1){
//        ALOGI("Didn't find a video stream.\n");
//        return -1;
//    }


    if (st_index[AVMEDIA_TYPE_VIDEO] < 0 || st_index[AVMEDIA_TYPE_VIDEO] > pFormatCtx->nb_streams) {
        return -1;
    }


    // AVStream* stream = pFormatCtx->streams[mVideoStreamIndex];
    // Get a pointer to the codec context for the video stream
    AVCodecContext* codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->codecpar);

    av_codec_set_pkt_timebase(codec_ctx, pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]]->time_base);

    streamVideo.dec_ctx = codec_ctx;
    streamVideo.st = pFormatCtx->streams[st_index[AVMEDIA_TYPE_VIDEO]];
    //AVCodecContext* codec_ctx = stream->codecpar
    AVCodec* codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (codec == NULL) {
        return -1;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec,NULL) < 0) {
        return -1;
    }

    mVideoWidth = codec_ctx->width;
    mVideoHeight = codec_ctx->height;
    mDuration =  pFormatCtx->duration;

    mConvertCtx = sws_getContext(codec_ctx->width,
                                 codec_ctx->height,
                                 codec_ctx->pix_fmt,
                                 codec_ctx->width,
                                 codec_ctx->height,
                                 AV_PIX_FMT_YUV420P,
                                 SWS_BICUBIC,
                                 NULL,
                                 NULL,
                                 NULL);

    if (mConvertCtx == NULL) {
        return -1;
    }

    mFrame = av_frame_alloc();
    if (mFrame == NULL) {
        return -1;
    }

    printf("YtxMediaPlayer::prepare OUT");

*/





 //-----------------------------------------------------

  //  AVPacket mPacket, *pPacket = &mPacket;

   // mCurrentState = MEDIA_PLAYER_STARTED;
    printf("playing %ix%i", mVideoWidth, mVideoHeight);
    int ret = 0;
    while (ret >= 0 )
    {
        usleep(400);

        ret = av_read_frame(pFormatCtx, packet);

    }




    return  0;
}



