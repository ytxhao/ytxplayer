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

//    char *gVertexShader = "attribute vec4 vPosition;\n"
//            "void main() {\n"
//            "  gl_Position = vPosition;\n"
//            "}\n";
//
//    char *gFragmentShader = "precision mediump float;\n"
//            "void main() {\n"
//            "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
//            "}\n";


  char* gVertexShader =
                    "attribute vec4 vPosition;\n"
                    "attribute vec2 a_texCoord;\n"
                    "varying vec2 tc;\n"
                    "void main() {\n"
                    "gl_Position = vPosition;\n"
                    "tc = a_texCoord;\n"
                    "}\n";

   char* gFragmentShader =
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

    GLfloat coord_buffer[8] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    GLfloat vertice_buffer[8] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
    };

    char *plane[3] = {0, 0, 0};

    GLuint yTextureId, uTextureId, vTextureId;
    int yHandle, uHandle, vHandle;
    int videoWidth;
    int videoHeight;

    GLuint program;
    GLuint gProgram;
    GLuint gvPositionHandle;
    GLuint gCoordHandle;

    static Lock mLock;
    static GlEngine *glEngine;
    static bool isInitComplete;

protected:
    GlEngine();
    ~GlEngine(){
        if(glEngine != NULL){
            delete glEngine;
        }
    }

public:
    static GlEngine *getGlEngine();
    static bool glEngineInitComplete(){
        return GlEngine::isInitComplete;
    }


    void printGLString(const char *name, GLenum s);

    void checkGlError(const char *op);

    GLuint loadShader(GLenum shaderType, const char *pSource);

    GLuint createProgram(const char *pVertexSource, const char *pFragmentSource);

    bool setupGraphics();

    void buildTextures(char *y, char *u, char *v, int videoWidth, int videoHeight);

    void drawFrame();

    void notifyRendererFrame(char *y, char *u, char *v, int videoWidth, int videoHeight);

    void renderFrameTest();

    bool setupGraphicsTest(int w, int h);

};


#endif //YTXPLAYER_GL_ENGINE_H
