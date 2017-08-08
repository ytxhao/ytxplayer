//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_SDLSTDINC_H
#define YTXPLAYER_SDLSTDINC_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int8_t      Sint8;
typedef uint8_t     Uint8;
typedef int16_t     Sint16;
typedef uint16_t    Uint16;
typedef int32_t     Sint32;
typedef uint32_t    Uint32;
typedef int64_t     Sint64;
typedef uint64_t    Uint64;

char *SDL_getenv(const char *name);
#endif //YTXPLAYER_SDLSTDINC_H
