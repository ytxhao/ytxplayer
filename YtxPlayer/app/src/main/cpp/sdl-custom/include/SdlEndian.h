//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_SDLENDIAN_H
#define YTXPLAYER_SDLENDIAN_H
#ifdef __cplusplus
extern "C" {
#endif
#define SDL_LIL_ENDIAN  1234
#define SDL_BIG_ENDIAN  4321

#include <endian.h>

#define SDL_BYTEORDER  __BYTE_ORDER
#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_SDLENDIAN_H
