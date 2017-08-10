//
// Created by Administrator on 2017/8/9.
//
#include "SdlAudio.h"

void SDL_CalculateAudioSpec(SDL_AudioSpec * spec)
{
    switch (spec->format) {
        case AUDIO_U8:
            spec->silence = 0x80;
            break;
        default:
            spec->silence = 0x00;
            break;
    }
    spec->size = SDL_AUDIO_BITSIZE(spec->format) / 8;
    spec->size *= spec->channels;
    spec->size *= spec->samples;
}

void SDL_MixAudio(Uint8*       dst,
                  const Uint8* src,
                  Uint32       len,
                  int          volume)
{
    // do nothing;
}
