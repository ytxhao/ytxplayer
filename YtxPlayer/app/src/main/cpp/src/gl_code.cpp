/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    ALOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        ALOGI("after %s() glError (0x%x)\n", op, error);
    }
}

//auto gVertexShader =
//    "attribute vec4 vPosition;\n"
//    "void main() {\n"
//    "  gl_Position = vPosition;\n"
//    "}\n";
//
//auto gFragmentShader =
//    "precision mediump float;\n"
//    "void main() {\n"
//    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
//    "}\n";

auto gVertexShader =
        "attribute vec4 vPosition;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 tc;\n"
        "void main() {\n"
        "gl_Position = vPosition;\n"
        "tc = a_texCoord;\n"
        "}\n";

auto gFragmentShader =
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

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    ALOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}
GLuint program;
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    ALOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;
GLuint gCoordHandle;

int yHandle = -1, uHandle = -1, vHandle = -1;

bool setupGraphics() {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    //ALOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        ALOGE("Could not create program.");
        return false;
    }
    /*
     * get handle for "vPosition" and "a_texCoord"
     */
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    ALOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);
    if(gvPositionHandle == -1){
        ALOGE("setupGraphics error gvPositionHandle =%d\n",gvPositionHandle);
    }

    gCoordHandle = glGetAttribLocation(gProgram, "a_texCoord");
    checkGlError("glGetAttribLocation");

    if(gCoordHandle == -1){
        ALOGE("setupGraphics error gCoordHandle = %d\n", gCoordHandle);
    }

    /*
     * get uniform location for y/u/v, we pass data through these uniforms
     */

    yHandle = glGetUniformLocation(gProgram, "tex_y");
    ALOGI("GLProgram yHandle = %d\n",yHandle);
    checkGlError("glGetUniformLocation tex_y");
    if (yHandle == -1) {
        ALOGE("Could not get uniform location for tex_y");
    }
    uHandle = glGetUniformLocation(gProgram, "tex_u");
    ALOGI("GLProgram uHandle = %d\n",uHandle);
    checkGlError("glGetUniformLocation tex_u");
    if (uHandle == -1) {
        ALOGE("Could not get uniform location for tex_u");
    }
    vHandle = glGetUniformLocation(gProgram, "tex_v");
    ALOGI("GLProgram vHandle = %d\n",vHandle);
    checkGlError("glGetUniformLocation tex_v");
    if (vHandle == -1) {
        ALOGE("Could not get uniform location for tex_v");
    }

   // glViewport(0, 0, w, h);
   // checkGlError("glViewport");
    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f };

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}
// y texture id  = ytid
GLuint  ytid = -1, utid = -1, vtid = -1;
char* plane[3]={0,0,0};
int videoWidth;
int videoHeight;

void buildTextures(char *y,char *u,char *v, int video_width, int video_height){

    // building texture for Y data
    if(ytid < 0){
        glGenTextures(1,&ytid);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures ytid=%d\n",ytid);
        checkGlError("glGenTextures");
    }
    glBindTexture(GL_TEXTURE_2D, ytid);
    checkGlError("glBindTexture");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, video_width, video_height, 0,
            GL_LUMINANCE, GL_UNSIGNED_BYTE, y);
    checkGlError("glTexImage2D");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // building texture for U data
    if(utid < 0){
        glGenTextures(1,&utid);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures utid=%d\n",utid);
        checkGlError("glGenTextures");
    }
    glBindTexture(GL_TEXTURE_2D,utid);
    checkGlError("glBindTexture");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, video_width / 2, video_width / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if(vtid < 0){
        glGenTextures(1,&vtid);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures utid=%d\n",vtid);
        checkGlError("glGenTextures");
    }

    glBindTexture(GL_TEXTURE_2D, vtid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, video_width / 2, video_width / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

static const GLfloat coord_buffer[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
};

static const GLfloat vertice_buffer[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
};

pthread_mutex_t mutex;
pthread_cond_t cond;

void updateYUV(char *y,char *u,char *v, int video_width, int video_height)
{

    plane[0]=y;
    plane[1]=u;
    plane[2]=v;
    videoWidth =video_width;
    videoHeight = video_height;


}
/**
 * render the frame
 * the YUV data will be converted to RGB by shader.
 */
void drawFrame(){

    if(plane[0]!=0,plane[1]!=0,plane[2]!=0) {

        buildTextures(plane[0], plane[1], plane[2], videoWidth, videoHeight);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        checkGlError("glUseProgram");

        glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, false, 8, vertice_buffer);
        checkGlError("glVertexAttribPointer mPositionHandle");
        glEnableVertexAttribArray(gvPositionHandle);

        glVertexAttribPointer(gCoordHandle, 2, GL_FLOAT, false, 8, coord_buffer);
        checkGlError("glVertexAttribPointer maTextureHandle");
        glEnableVertexAttribArray(gCoordHandle);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ytid);
        glUniform1i(yHandle, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, utid);
        glUniform1i(uHandle, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, vtid);
        glUniform1i(vHandle, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFinish();

        glDisableVertexAttribArray(gvPositionHandle);
        glDisableVertexAttribArray(gCoordHandle);
    }

}
extern "C" {
//    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
//    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);

JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1resize_1opengl(JNIEnv *env, jclass clazz, jint width, jint height);
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1step_1opengl(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1init_1opengl(JNIEnv *env, jclass clazz);

};


JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1resize_1opengl(JNIEnv *env, jclass clazz, jint width, jint height)
{

    ALOGI("native_1resize_1opengl IN out");
}
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1step_1opengl(JNIEnv *env, jclass clazz)
{
    //renderFrame();
    ALOGI("native_1step_1opengl IN out");
    drawFrame();
}
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1init_1opengl(JNIEnv *env, jclass clazz)
{
    ALOGI("native_1init_1opengl IN out");
    //创建program
    setupGraphics();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

}



//JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
//{
//    setupGraphics(width, height);
//}
//
//JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
//{
//    renderFrame();
//}
