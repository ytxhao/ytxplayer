//
// Created by Administrator on 2016/9/22.
//
#define TAG "FFMpegAudioDecoder"

#include "ytxplayer/ALog-priv.h"

#include <ytxplayer/VideoStateInfo.h>
#include <ytxplayer/ffmsg.h>
#include "ytxplayer/decoder_audio.h"


DecoderAudio::DecoderAudio(VideoStateInfo *mVideoStateInfo) : IDecoder(mVideoStateInfo) {

    mSamples = NULL;
    mSamplesSize = 0;
    mFrame = NULL;
    af = NULL;
    next_pts = 0;
    lastStats = false;
    curStats = false;
    firstInit = false;
    mConvertCtx = NULL;
    mVideoStateInfo->initClock(mVideoStateInfo->audClk, &mQueue->serial);
    mVideoStateInfo->setClockSpeed(mVideoStateInfo->audClk, 1);

}

DecoderAudio::~DecoderAudio() {
    // Free audio samples buffer
    if (mFrame != NULL) {
        av_frame_free(&mFrame);
    }

    if (!mConvertCtx) {
        swr_free(&mConvertCtx);
    }

    avcodec_close(mVideoStateInfo->streamAudio->dec_ctx);
}

bool DecoderAudio::prepare() {

    mConvertCtx = swr_alloc();
    swr_alloc_set_opts(mConvertCtx,
                       mVideoStateInfo->out_ch_layout, mVideoStateInfo->out_sample_fmt,
                       mVideoStateInfo->out_sample_rate,
                       mVideoStateInfo->in_ch_layout, mVideoStateInfo->in_sample_fmt,
                       mVideoStateInfo->in_sample_rate,
                       0, NULL);


    swr_init(mConvertCtx);
    //   16bit 44100 PCM 数据
    mVideoStateInfo->out_buffer_audio = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE);
    mFrame = av_frame_alloc();


    if (mFrame == NULL) {
        return false;
    }

    return true;
}

bool DecoderAudio::process(MAVPacket *mPacket) {
    int pts = 0;
    int size = mSamplesSize;
    int completed;

    curStats = mPacket->isEnd;

    if (curStats != lastStats && curStats && mPacket->pkt.data == NULL) {
        msg.what = FFP_MSG_COMPLETED;
        mVideoStateInfo->messageQueueAudio->put(&msg);
        lastStats = curStats;
        return true;
    }
    lastStats = curStats;

    if (mQueue->size() == 0) {
        pthread_cond_signal(&mVideoStateInfo->continue_read_thread);
    }

    if (mPacket->pkt.data == mVideoStateInfo->flushPkt->pkt.data) {
        avcodec_flush_buffers(mVideoStateInfo->streamAudio->dec_ctx);
        mVideoStateInfo->isFirstAudioFrame = true;
        return true;
    }

    if (mVideoStateInfo->pkt_serial_audio != mQueue->serial) {
        return true;
    }


    if (mPacket->pkt.size == 0 && mPacket->pkt.data == NULL) {
        return true;
    }

    int ret = avcodec_decode_audio4(mVideoStateInfo->streamAudio->dec_ctx, mFrame, &completed,
                                    &mPacket->pkt);

    if (completed > 0) {
        //   pts = synchronize(mFrame, pts);

        // onDecode(mFrame, pts);

        tb = (AVRational) {1, mFrame->sample_rate};

        if (mFrame->pts != AV_NOPTS_VALUE) {
            mFrame->pts = av_rescale_q(mFrame->pts,
                                       mVideoStateInfo->streamAudio->dec_ctx->time_base, tb);
        } else if (mFrame->pkt_pts != AV_NOPTS_VALUE) {
            mFrame->pts = av_rescale_q(mFrame->pkt_pts, av_codec_get_pkt_timebase(
                    mVideoStateInfo->streamAudio->dec_ctx), tb);
        } else if (next_pts != AV_NOPTS_VALUE) {
            mFrame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
        }

        if (mFrame->pts != AV_NOPTS_VALUE) {
            next_pts = mFrame->pts + mFrame->nb_samples;
            next_pts_tb = tb;
        }

        if (!(af = mVideoStateInfo->frameQueueAudio->frameQueuePeekWritable())) {
            return true;
        }

        tb = (AVRational) {1, mFrame->sample_rate};
        af->pts = (mFrame->pts == AV_NOPTS_VALUE) ? NAN : mFrame->pts * av_q2d(tb);
        af->pos = av_frame_get_pkt_pos(mFrame);
        af->serial = mVideoStateInfo->pkt_serial_audio;
        af->duration = av_q2d((AVRational) {mFrame->nb_samples, mFrame->sample_rate});




        swr_convert(mConvertCtx, &(mVideoStateInfo->out_buffer_audio), MAX_AUDIO_FRAME_SIZE,
                    (const uint8_t **) mFrame->data, mFrame->nb_samples);
        //获取sample的size
        af->out_buffer_audio_size = av_samples_get_buffer_size(NULL,
                                                               mVideoStateInfo->out_channel_nb,
                                                               mFrame->nb_samples,
                                                               mVideoStateInfo->out_sample_fmt,
                                                               1);

        af->out_buffer_audio = (uint8_t *) av_malloc(af->out_buffer_audio_size);


        memcpy(af->out_buffer_audio, mVideoStateInfo->out_buffer_audio, af->out_buffer_audio_size);

        av_frame_move_ref(af->frame, mFrame);
        mVideoStateInfo->frameQueueAudio->frameQueuePush();
        //av_frame_unref(mFrame);
//        if (mVideoStateInfo->isFirstAudioFrame && mVideoStateInfo->frameQueueAudio->windex > 3) {
//            mVideoStateInfo->isFirstAudioFrame = false;
//            msg.what = FFP_MSG_AUDIO_FIRST_FRAME;
//            mVideoStateInfo->messageQueueAudio->put(&msg);
//        }
    }

    return true;
}

bool DecoderAudio::decode(void *ptr) {
    int i;
    MAVPacket pPacket;

    ALOGI(TAG, "decoding audio");

    while (mRunning) {
        if (mQueue->get(&pPacket, true, &mVideoStateInfo->pkt_serial_audio) < 0) {
            mRunning = false;
            return false;
        }
        if (!process(&pPacket)) {
            mRunning = false;
            return false;
        }
        // Free the packet that was allocated by av_read_frame
        av_packet_unref(&pPacket.pkt);
    }

    ALOGI(TAG, "decoding audio ended");

    return true;
}

void DecoderAudio::stop() {
    mRunning = false;
    mQueue->abort();
    mVideoStateInfo->frameQueueAudio->frameQueueReset();
    ALOGI("waiting on end of decoder thread\n");
    int ret = -1;
    if ((ret = wait()) != 0) {
        ALOGE("Couldn't cancel IDecoder: %i\n", ret);
        return;
    }
}

int DecoderAudio::streamHasEnoughPackets() {
    int ret = 0;
    ret = mVideoStateInfo->st_index[AVMEDIA_TYPE_AUDIO] < 0 ||
          mQueue->mAbortRequest ||
          (mVideoStateInfo->streamAudio->st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
          mQueue->size() > MIN_FRAMES && (!mQueue->duration ||
                                          av_q2d(mVideoStateInfo->streamAudio->st->time_base) *
                                          mQueue->duration > 1.0);

    return ret;
}

static void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
    //ALOGI("sdl_audio_callback");
    DecoderAudio *mDecoderAudio = (DecoderAudio *) opaque;

    int audio_size;
    int len1;
    if (mDecoderAudio->mVideoStateInfo == NULL) {
        memset(stream, 0, len);
        return;
    }


    mDecoderAudio->mVideoStateInfo->audio_callback_time = av_gettime_relative();

    if (mDecoderAudio->mVideoStateInfo->pf_playback_rate_changed) {
        mDecoderAudio->mVideoStateInfo->pf_playback_rate_changed = 0;
        SDL_AoutSetPlaybackRate(mDecoderAudio->mVideoStateInfo->aout, mDecoderAudio->mVideoStateInfo->pf_playback_rate);
    }
    if (mDecoderAudio->mVideoStateInfo->pf_playback_volume_changed) {
        mDecoderAudio->mVideoStateInfo->pf_playback_volume_changed = 0;
        SDL_AoutSetPlaybackVolume(mDecoderAudio->mVideoStateInfo->aout, mDecoderAudio->mVideoStateInfo->pf_playback_volume);
    }

    while (len > 0) {
        if (mDecoderAudio->mVideoStateInfo->audio_buf_index >= mDecoderAudio->mVideoStateInfo->audio_buf_size) {
            audio_size = mDecoderAudio->mVideoStateInfo->audioDecodeFrame();
            if (audio_size < 0) {
                /* if error, just output silence */
                mDecoderAudio->mVideoStateInfo->audio_buf = NULL;
                mDecoderAudio->mVideoStateInfo->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / mDecoderAudio->mVideoStateInfo->audio_tgt.frame_size * mDecoderAudio->mVideoStateInfo->audio_tgt.frame_size;
            } else {
                mDecoderAudio->mVideoStateInfo->audio_buf_size = audio_size;
            }
            mDecoderAudio->mVideoStateInfo->audio_buf_index = 0;
        }

        if (mDecoderAudio->mVideoStateInfo->pkt_serial_audio != mDecoderAudio->mQueue->serial) {
            mDecoderAudio->mVideoStateInfo->audio_buf_index = mDecoderAudio->mVideoStateInfo->audio_buf_size;
            memset(stream, 0, len);
            // stream += len;
            // len = 0;
            SDL_AoutFlushAudio(mDecoderAudio->mVideoStateInfo->aout);
            break;
        }
        len1 = mDecoderAudio->mVideoStateInfo->audio_buf_size - mDecoderAudio->mVideoStateInfo->audio_buf_index;
        if (len1 > len)
            len1 = len;
        if (!mDecoderAudio->mVideoStateInfo->muted && mDecoderAudio->mVideoStateInfo->audio_buf && mDecoderAudio->mVideoStateInfo->audio_volume == SDL_MIX_MAXVOLUME) {
            memcpy(stream, mDecoderAudio->mVideoStateInfo->audio_buf +
                           mDecoderAudio->mVideoStateInfo->audio_buf_index, len1);
        } else {
            memset(stream, 0, len1);
            if (!mDecoderAudio->mVideoStateInfo->muted && mDecoderAudio->mVideoStateInfo->audio_buf) {
                SDL_MixAudio(stream, (uint8_t *) mDecoderAudio->mVideoStateInfo->audio_buf +
                                     mDecoderAudio->mVideoStateInfo->audio_buf_index, len1,
                             mDecoderAudio->mVideoStateInfo->audio_volume);
            }
        }
        len -= len1;
        stream += len1;
        mDecoderAudio->mVideoStateInfo->audio_buf_index += len1;
    }
    mDecoderAudio->mVideoStateInfo->audio_write_buf_size = mDecoderAudio->mVideoStateInfo->audio_buf_size - mDecoderAudio->mVideoStateInfo->audio_buf_index;
    /* Let's assume the audio driver that is used by SDL has two periods. */
    if (!isnan(mDecoderAudio->mVideoStateInfo->audio_clock)) {
//        set_clock_at(&is->audclk, is->audio_clock - (double)(is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec - SDL_AoutGetLatencySeconds(ffp->aout), is->audio_clock_serial, ffp->audio_callback_time / 1000000.0);
//        sync_clock_to_slave(&is->extclk, &is->audclk);

        double pts = mDecoderAudio->mVideoStateInfo->audio_clock - (double)(mDecoderAudio->mVideoStateInfo->audio_write_buf_size) / mDecoderAudio->mVideoStateInfo->audio_tgt.bytes_per_sec - SDL_AoutGetLatencySeconds(mDecoderAudio->mVideoStateInfo->aout);
        mDecoderAudio->mVideoStateInfo->setClockAt(mDecoderAudio->mVideoStateInfo->audClk, pts, mDecoderAudio->mVideoStateInfo->aFrame->serial,
                                                   mDecoderAudio->mVideoStateInfo->audio_callback_time / 1000000.0);
        mDecoderAudio->mVideoStateInfo->syncClock2Slave(mDecoderAudio->mVideoStateInfo->extClk, mDecoderAudio->mVideoStateInfo->audClk);
    }

    if (mDecoderAudio->mVideoStateInfo->st_index[AVMEDIA_TYPE_VIDEO] < 0) {
        mDecoderAudio->mVideoStateInfo->currentTime = (int) (mDecoderAudio->mVideoStateInfo->aFrame->pts * 1000);
    }

}

bool DecoderAudio::audioOpen(int64_t wanted_channel_layout, int wanted_nb_channels,
                             int wanted_sample_rate, struct AudioParams *audio_hw_params)
{

    SDL_AudioSpec wanted_spec, spec;
    static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
#ifdef FFP_MERGE
    static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
#endif
    static const int next_sample_rates[] = {0, 44100, 48000};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

    mVideoStateInfo->audio_volume = SDL_MIX_MAXVOLUME;
    mVideoStateInfo->muted = 0;

//    env = SDL_getenv("SDL_AUDIO_CHANNELS");
//    if (env) {
//        wanted_nb_channels = atoi(env);
//        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
//    }
    if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.freq = wanted_sample_rate;
    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
        ALOGE("Invalid sample rate or channel count!\n");
        return -1;
    }
    while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
        next_sample_rate_idx--;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AoutGetAudioPerSecondCallBacks(mVideoStateInfo->aout)));
    wanted_spec.callback = sdl_audio_callback;
    wanted_spec.userdata = this;
    while (SDL_AoutOpenAudio(mVideoStateInfo->aout, &wanted_spec, &spec) < 0) {
        /* avoid infinity loop on exit. --by bbcallen */
        if (mVideoStateInfo->abort_request)
            return -1;
        ALOGI("SDL_OpenAudio (%d channels, %d Hz)\n", wanted_spec.channels, wanted_spec.freq);
        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
        if (!wanted_spec.channels) {
            wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
            wanted_spec.channels = wanted_nb_channels;
            if (!wanted_spec.freq) {
                ALOGI("No more combinations to try, audio open failed\n");
                return -1;
            }
        }
        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
    }
    if (spec.format != AUDIO_S16SYS) {
        ALOGE("SDL advised audio format %d is not supported!\n", spec.format);
        return -1;
    }
    if (spec.channels != wanted_spec.channels) {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) {
            ALOGE("SDL advised channel count %d is not supported!\n", spec.channels);
            return -1;
        }
    }

    audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
    audio_hw_params->freq = spec.freq;
    audio_hw_params->channel_layout = wanted_channel_layout;
    audio_hw_params->channels =  spec.channels;
    audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
    audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
    if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
        ALOGE( "av_samples_get_buffer_size failed\n");
        return -1;
    }

    SDL_AoutSetDefaultLatencySeconds(mVideoStateInfo->aout, ((double)(2 * spec.size)) / audio_hw_params->bytes_per_sec);
    return spec.size;
}