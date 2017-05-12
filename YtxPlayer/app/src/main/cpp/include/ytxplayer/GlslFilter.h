//
// Created by Administrator on 2017/4/7.
//

#ifndef MEDIACODECTEST_GLSLFILTER_H
#define MEDIACODECTEST_GLSLFILTER_H


#include <stdint.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "MediaPlayerListener.h"
#include <png.h>
#include "MessageQueue.h"
#include "lock.h"
// ----------------------------------------------------------------------------
// for native window JNI
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>



typedef struct {

    GLuint texture;
    int width;
    int height;

}Picture;


class GlslFilter {

private:
    GLuint createProgram(const char *pVertexSource, const char *pFragmentSource);

public:
    GlslFilter();

    ~GlslFilter();

    void initial();
    void printGLString(const char *name, GLenum s);
    void checkGlError(const char *op);
    GLuint loadShader(GLenum shaderType, const char *pSource);
    void renderBackground();
    void process(VMessageData *vData);
    void addRendererFrame(VMessageData *vData);
    void addRendererFrameInit(VMessageData *vData);
    GLuint createTexture();
    void setViewPort(int mSurfaceWidth, int mSurfaceHeight);
    void setScreenWidth(int mScreenWidth);

    void setScreenHeight(int mScreenHeight);
    void buildTextures();
    void drawFrame();
    FILE *fp_yuv;
    void setAspectRatio();

    const char *getVertexShaderString();

    const char *getFragmentShaderString();

    bool isInitialed = false;
    GLuint vertexShader;
    GLuint pixelShader;
    GLuint shaderProgram;
    Lock mLock;
    image_t *img;
    image_t *gen_image(int width, int height);
    char *plane[3] = {NULL, NULL, NULL};
    GLint texCoordHandle;
    GLint posCoordHandle;
    int videoWidth = 0;
    int videoHeight = 0;
    GLuint yTextureId, uTextureId, vTextureId, pngTextureId;
    int yHandle, uHandle, vHandle, pngHandle;
    int mScreenWidth;
    int mScreenHeight;

    GLfloat posVertices[8] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
    };

    GLfloat texVertices[8] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,

    };

};














#endif //MEDIACODECTEST_GLSLFILTER_H
