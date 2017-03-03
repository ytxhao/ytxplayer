//
// Created by Administrator on 2017/3/3.
//

#ifndef YTXPLAYER_UTILS_H
#define YTXPLAYER_UTILS_H

#include "ffinc.h"


void *grow_array(void *array, int elem_size, int *size, int new_size);

#define GROW_ARRAY(array, nb_elems)\
     grow_array(array, sizeof(*array), &nb_elems, nb_elems + 1)


double get_rotation(AVStream *st);


#endif //YTXPLAYER_UTILS_H
