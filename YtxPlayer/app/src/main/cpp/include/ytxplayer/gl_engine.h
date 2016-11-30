//
// Created by Administrator on 2016/11/17.
//

#ifndef YTXPLAYER_GL_ENGINE_H
#define YTXPLAYER_GL_ENGINE_H

// OpenGL ES 2.0 code

#include <jni.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "lock.h"


class GlEngine {

private:
//    char* gFragmentShader =
//            "precision mediump float;\n"
//                    "uniform sampler2D tex_y;\n"
//                    "uniform sampler2D tex_u;\n"
//                    "uniform sampler2D tex_v;\n"
//                    "varying vec2 tc;\n"
//                    "void main() {\n"
//                    "vec3 yuv;\n"
//                    "vec3 rgb;\n"
//                    "yuv.x = texture2D(tex_y, tc).r;\n"
//                    "yuv.y = texture2D(tex_u, tc).r - 0.5;\n"
//                    "yuv.z = texture2D(tex_v, tc).r - 0.5;\n"
//                    "rgb = mat3(1,       1,         1,0,       -0.39465,  2.03211,1.13983, -0.58060,  0) * yuv;\n"
//                    "gl_FragColor = vec4(rgb, 1);\n"
//                    "}\n";


//    Y’ = 0.257*R' + 0.504*G' + 0.098*B' + 16
//    Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
//    Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
//    R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
//    G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
//    B' = 1.164*(Y’-16) + 2.017*(Cb'-128)

    char *gVertexShader =
            "attribute vec4 vPosition;\n"
                    "attribute vec2 a_texCoord;\n"
                    "varying vec2 tc;\n"
                    "void main() {\n"
                    "gl_Position = vPosition;\n"
                    "tc = a_texCoord;\n"
                    "}\n";

    char *gFragmentShader =
            "precision mediump float;\n"
                    "uniform sampler2D tex_y;\n"
                    "uniform sampler2D tex_u;\n"
                    "uniform sampler2D tex_v;\n"
                    "varying vec2 tc;\n"
                    "void main() {\n"
                    "vec4 c = vec4((texture2D(tex_y, tc).r - 16./255.) * 1.164);\n"
                    "vec4 U = vec4(texture2D(tex_u, tc).r - 128./255.);\n"
                    "vec4 V = vec4(texture2D(tex_v, tc).r - 128./255.);\n"
                    "c += V * vec4(1.596, -0.813, 0, 0);\n"
                    "c += U * vec4(0, -0.392, 2.017, 0);\n"
                    "c.a = 1.0;\n"
                    "gl_FragColor = c;\n"
                    "}\n";


    GLfloat gTriangleVertices[6] = {0.0f, 0.5f, -0.5f, -0.5f,
                                    0.5f, -0.5f};

    //    "a_texCoord" //texcoord 是纹理坐标，在后续的Pixel shader中会用到用来读取纹理颜色
    GLfloat coord_buffer[8] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    //    "vPosition"
    GLfloat vertice_buffer[8] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
    };

    char *plane[3] = {NULL, NULL, NULL};

    GLuint yTextureId, uTextureId, vTextureId;
    int yHandle, uHandle, vHandle;
    int videoWidth = 0;
    int videoHeight = 0;
    int mScreenWidth = 720;
    int mScreenHeight = 1080;

    GLuint program;
    GLuint gProgram;
    GLuint gvPositionHandle;
    GLuint gCoordHandle;
    Lock   mRendererLock;
    bool   isFrameRendererFinish;

    static Lock mLock;
    static GlEngine *glEngine;
    static bool isInitComplete;

protected:
    GlEngine();

    ~GlEngine();

public:
    static GlEngine *getGlEngine();

    static bool glEngineInitComplete() {
        return GlEngine::isInitComplete;
    }

    static void glSetEngineInitComplete(bool isComplete) {
        GlEngine::isInitComplete = isComplete;
    }

    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    GLuint loadShader(GLenum shaderType, const char *pSource);

    GLuint createProgram(const char *pVertexSource, const char *pFragmentSource);

    bool setupGraphics();

    bool isSetupGraphics;

    void buildTextures();

    void drawFrame();

    void drawFrameInit(int videoWidth, int videoHeight);

    bool isDrawFrameInit;

    void addRendererFrame(char *y, char *u, char *v, int videoWidth, int videoHeight);

    bool isAddRendererFrameInit;

    void addRendererFrameInit(int videoWidth, int videoHeight);

    void setAspectRatio();

    void setVideoWidthAndHeight(int videoWidth, int videoHeight);

    void renderFrameTest();

    bool setupGraphicsTest(int w, int h);

    void setScreenWidth(int mScreenWidth);

    void setScreenHeight(int mScreenHeight);

    int getScreenWidth();

    int getScreenHeight();

    void waitRendererFinish();

    void signalRendererFinish();

    void setViewPort(int mSurfaceWidth,int mSurfaceHeight);



};


#endif //YTXPLAYER_GL_ENGINE_H
