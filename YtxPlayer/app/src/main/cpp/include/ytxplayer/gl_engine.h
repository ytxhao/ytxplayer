//
// Created by Administrator on 2016/11/17.
//

#ifndef YTXPLAYER_GL_ENGINE_H
#define YTXPLAYER_GL_ENGINE_H

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "ALog-priv.h"
#include "gl_code.h"

#define  TAG    "gl2jni"

class GlEngine{

private:
    const char* gVertexShader = "attribute vec4 vPosition;\n"
                    "void main() {\n"
                    "  gl_Position = vPosition;\n"
                    "}\n";

    const char* gFragmentShader = "precision mediump float;\n"
                    "void main() {\n"
                    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                    "}\n";

    const GLfloat gTriangleVertices[6] = { 0.0f, 0.5f, -0.5f, -0.5f,
                                          0.5f, -0.5f };

    const GLfloat coord_buffer[8] = {
            0.0f,  1.0f,
            1.0f,  1.0f,
            0.0f,  0.0f,
            1.0f,  0.0f,
    };

    const GLfloat vertice_buffer[8] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            1.0f,  1.0f,
    };

    char* plane[3]={0,0,0};

    GLuint  yTextureId, uTextureId , vTextureId ;

    int videoWidth;
    int videoHeight;

    GLuint program;
    GLuint gProgram;
    GLuint gvPositionHandle;
    GLuint gCoordHandle;
    int yHandle = -1, uHandle = -1, vHandle = -1;

    pthread_mutex_t mutex;
    pthread_cond_t cond;


public:
    GlEngine();
    void printGLString(const char *name, GLenum s);
    void checkGlError(const char* op);
    GLuint loadShader(GLenum shaderType, const char* pSource);
    GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
    bool setupGraphics();
    void buildTextures(char *y,char *u,char *v, int video_width, int video_height);
    void drawFrame();



};


#endif //YTXPLAYER_GL_ENGINE_H
