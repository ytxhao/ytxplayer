//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_SDLAOUT_H
#define YTXPLAYER_SDLAOUT_H
#include "SdlAudio.h"
#include "SdlClass.h"
#include "SdlMutex.h"

typedef struct SDL_Aout_Opaque SDL_Aout_Opaque;
typedef struct SDL_Aout SDL_Aout;
struct SDL_Aout {
    SDL_mutex *mutex;
    double     minimal_latency_seconds;

    SDL_Class       *opaque_class;
    SDL_Aout_Opaque *opaque;
    void (*free_l)(SDL_Aout *vout);
    int (*open_audio)(SDL_Aout *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
    void (*pause_audio)(SDL_Aout *aout, int pause_on);
    void (*flush_audio)(SDL_Aout *aout);
    void (*set_volume)(SDL_Aout *aout, float left, float right);
    void (*close_audio)(SDL_Aout *aout);

    double (*func_get_latency_seconds)(SDL_Aout *aout);
    void   (*func_set_default_latency_seconds)(SDL_Aout *aout, double latency);

    // optional
    void   (*func_set_playback_rate)(SDL_Aout *aout, float playbackRate);
    void   (*func_set_playback_volume)(SDL_Aout *aout, float playbackVolume);
    int    (*func_get_audio_persecond_callbacks)(SDL_Aout *aout);

    // Android only
    int    (*func_get_audio_session_id)(SDL_Aout *aout);
};

int SDL_AoutOpenAudio(SDL_Aout *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
void SDL_AoutPauseAudio(SDL_Aout *aout, int pause_on);
void SDL_AoutFlushAudio(SDL_Aout *aout);
void SDL_AoutSetStereoVolume(SDL_Aout *aout, float left_volume, float right_volume);
void SDL_AoutCloseAudio(SDL_Aout *aout);
void SDL_AoutFree(SDL_Aout *aout);
void SDL_AoutFreeP(SDL_Aout **paout);

double SDL_AoutGetLatencySeconds(SDL_Aout *aout);
void   SDL_AoutSetDefaultLatencySeconds(SDL_Aout *aout, double latency);
int    SDL_AoutGetAudioPerSecondCallBacks(SDL_Aout *aout);

// optional
void   SDL_AoutSetPlaybackRate(SDL_Aout *aout, float playbackRate);
void   SDL_AoutSetPlaybackVolume(SDL_Aout *aout, float volume);

// android only
int    SDL_AoutGetAudioSessionId(SDL_Aout *aout);
#endif //YTXPLAYER_SDLAOUT_H
