//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_AOUTOPENSLES_H
#define YTXPLAYER_AOUTOPENSLES_H

#include "SdlAout.h"

SDL_Aout *SDL_AoutAndroid_CreateForOpenSLES();
bool      SDL_AoutAndroid_IsObjectOfOpenSLES(SDL_Aout *aout);
#endif //YTXPLAYER_AOUTOPENSLES_H
