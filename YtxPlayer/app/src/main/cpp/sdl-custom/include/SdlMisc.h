//
// Created by Administrator on 2017/7/17.
//

#ifndef YTXPLAYER_SDLMISC_H
#define YTXPLAYER_SDLMISC_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <memory.h>

#ifndef YTXMAX
#define YTXMAX(a, b)    ((a) > (b) ? (a) : (b))
#endif

#ifndef YTXMIN
#define YTXMIN(a, b)    ((a) < (b) ? (a) : (b))
#endif

#ifndef YTXALIGN
#define YTXALIGN(x, align) ((( x ) + (align) - 1) / (align) * (align))
#endif

#define YTX_CHECK_RET(condition__, retval__, ...) \
    if (!(condition__)) { \
        ALOGE(__VA_ARGS__); \
        return (retval__); \
    }

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

inline static void *mallocz(size_t size) {
    void *mem = malloc(size);
    if (!mem)
        return mem;

    memset(mem, 0, size);
    return mem;
}

inline static void freep(void **mem) {
    if (mem && *mem) {
        free(*mem);
        *mem = NULL;
    }
}

#ifdef __cplusplus
}
#endif
#endif //YTXPLAYER_SDLMISC_H
