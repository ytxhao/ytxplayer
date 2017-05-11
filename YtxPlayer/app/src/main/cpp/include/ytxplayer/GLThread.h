//
// Created by Administrator on 2017/4/11.
//

#ifndef MEDIACODECTEST_GLTHREAD_H
#define MEDIACODECTEST_GLTHREAD_H

#include "VideoStateInfo.h"
#include "Thread.h"
#include "headerBmp.h"
#include "lock.h"
#include "GlslFilter.h"
#include "ffinc.h"
#define GL_MSG_CANCEL               0
#define GL_MSG_RENDERER             1
#define GL_MSG_DECODED_FIRST_FRAME   2

class GLThread : public Thread {

public:
    GLThread(VideoStateInfo *mVideoStateInfo);

    ~GLThread();

    void handleRun(void *ptr);

    bool prepare();

    void refresh();

    bool process(AVMessage *msg);

    void stop();

    void initEGL(int width, int height);
    void drawGL(GlslFilter *filter,VMessageData *vData );
    void deInitEGL();
    void enqueue(AVMessage *msg);
    void SnapshotBmpRGBA(GLvoid * pData, int width, int height,  char * filename);
    void SnapshotBmpRGB(GLvoid * pData, int width, int height,  char * filename);

    void addRendererFrame(image_t *img, char *y, char *u, char *v, int videoWidth, int videoHeight);

    VideoStateInfo *mVideoStateInfo = NULL;
    GlslFilter *glslFilter=NULL;
    Picture *mPicture;
    Picture *mTexturePicture;
    int texture;

    EGLConfig eglConf;
    EGLSurface eglSurface;
    EGLContext eglCtx;
    EGLDisplay eglDisp;
    void *RGBABuffer;
    int times=0;
    Lock mLock;

};

#endif //MEDIACODECTEST_GLTHREAD_H
