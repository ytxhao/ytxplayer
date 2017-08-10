//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_AOUTOPENSLES_H
#define YTXPLAYER_AOUTOPENSLES_H
#ifdef __cplusplus
extern "C" {
#endif

#include "SdlAout.h"

SDL_Aout *SDL_AoutAndroid_CreateForOpenSLES();

bool SDL_AoutAndroid_IsObjectOfOpenSLES(SDL_Aout *aout);

#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_AOUTOPENSLES_H
