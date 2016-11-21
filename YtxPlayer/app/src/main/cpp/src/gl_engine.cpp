#define  TAG  "gl2jni"

#include "ALog-priv.h"
#include "gl_engine.h"

GlEngine::GlEngine() {

    yTextureId = -1;
    uTextureId = -1;
    vTextureId = -1;
    yHandle = -1;
    uHandle = -1;
    vHandle = -1;


}

void GlEngine::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    ALOGI("GL %s = %s\n", name, v);
}

void GlEngine::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        ALOGI("after %s() glError (0x%x)\n", op, error);
    }
}


GLuint GlEngine::loadShader(GLenum shaderType, const char *pSource) {

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
                char *buf = (char *) malloc(infoLen);
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

GLuint GlEngine::createProgram(const char *pVertexSource, const char *pFragmentSource) {
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
                char *buf = (char *) malloc(bufLength);
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


bool GlEngine::setupGraphics() {
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
    if (gvPositionHandle == -1) {
        ALOGE("setupGraphics error gvPositionHandle =%d\n", gvPositionHandle);
    }

    gCoordHandle = glGetAttribLocation(gProgram, "a_texCoord");
    checkGlError("glGetAttribLocation");

    if (gCoordHandle == -1) {
        ALOGE("setupGraphics error gCoordHandle = %d\n", gCoordHandle);
    }

    /*
     * get uniform location for y/u/v, we pass data through these uniforms
     */

    yHandle = glGetUniformLocation(gProgram, "tex_y");
    ALOGI("GLProgram yHandle = %d\n", yHandle);
    checkGlError("glGetUniformLocation tex_y");
    if (yHandle == -1) {
        ALOGE("Could not get uniform location for tex_y");
    }
    uHandle = glGetUniformLocation(gProgram, "tex_u");
    ALOGI("GLProgram uHandle = %d\n", uHandle);
    checkGlError("glGetUniformLocation tex_u");
    if (uHandle == -1) {
        ALOGE("Could not get uniform location for tex_u");
    }
    vHandle = glGetUniformLocation(gProgram, "tex_v");
    ALOGI("GLProgram vHandle = %d\n", vHandle);
    checkGlError("glGetUniformLocation tex_v");
    if (vHandle == -1) {
        ALOGE("Could not get uniform location for tex_v");
    }

    // glViewport(0, 0, w, h);
    // checkGlError("glViewport");
    return true;
}


void GlEngine::buildTextures(char *y, char *u, char *v, int videoWidth, int videoHeight) {
    // building texture for Y data
    if (yTextureId < 0) {
        glGenTextures(1, &yTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures yTextureId=%d\n", yTextureId);
        checkGlError("glGenTextures");
    }
    glBindTexture(GL_TEXTURE_2D, yTextureId);
    checkGlError("glBindTexture");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth, videoHeight, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, y);
    checkGlError("glTexImage2D");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // building texture for U data
    if (uTextureId < 0) {
        glGenTextures(1, &uTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures uTextureId=%d\n", uTextureId);
        checkGlError("glGenTextures");
    }
    glBindTexture(GL_TEXTURE_2D, uTextureId);
    checkGlError("glBindTexture");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth / 2, videoHeight / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE, u);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (vTextureId < 0) {
        glGenTextures(1, &vTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        ALOGI("buildTextures vTextureId=%d\n", vTextureId);
        checkGlError("glGenTextures");
    }

    glBindTexture(GL_TEXTURE_2D, vTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth / 2, videoHeight / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE, v);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}


/**
 * render the frame
 * the YUV data will be converted to RGB by shader.
 */
void GlEngine::drawFrame() {
    if (plane[0] != NULL && plane[1] != NULL && plane[2] != NULL) {

     //   setAspectRatio();


        glViewport(0, 0, videoWidth, videoHeight);
        checkGlError("glViewport");
        ALOGI("drawFrame videoWidth=%d videoHeight=%d\n", videoWidth, videoHeight);
        buildTextures(plane[0], plane[1], plane[2], videoWidth, videoHeight);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
        glBindTexture(GL_TEXTURE_2D, yTextureId);
        glUniform1i(yHandle, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uTextureId);
        glUniform1i(uHandle, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, vTextureId);
        glUniform1i(vHandle, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFinish();

        glDisableVertexAttribArray(gvPositionHandle);
        glDisableVertexAttribArray(gCoordHandle);
    }

}

void GlEngine::addRendererFrame(char *y, char *u, char *v, int videoWidth, int videoHeight) {
    plane[0] = y;
    plane[1] = u;
    plane[2] = v;
    this->videoWidth = videoWidth;
    this->videoHeight = videoHeight;
}

void GlEngine::setAspectRatio(){
    float f1 = (float) mScreenHeight / mScreenWidth;
    float f2 = (float)  videoHeight / videoWidth;
    float widthScale = 0.0;
    float heightScale = 0.0;
    if (f1 == f2) {

    } else if (f1 < f2) {
        widthScale =  f1 / f2;
        ALOGI("setAspectRatio widthScale=%f\n", widthScale);
        vertice_buffer[0] = -widthScale;
        vertice_buffer[1] = -1.0f;
        vertice_buffer[2] = widthScale;
        vertice_buffer[3] = -1.0f;
        vertice_buffer[4] = -widthScale;
        vertice_buffer[5] = 1.0f;
        vertice_buffer[6] = widthScale;
        vertice_buffer[7] = 1.0f;


    } else {
        heightScale = f2 / f1;
        ALOGI("setAspectRatio heightScale=%f\n", heightScale);
        coord_buffer[0] = -1.0f;
        coord_buffer[1] = -heightScale;
        coord_buffer[2] = 1.0f;
        coord_buffer[3] = -heightScale;
        coord_buffer[4] = -1.0f;
        coord_buffer[5] = heightScale;
        coord_buffer[6] = 1.0f;
        coord_buffer[7] = heightScale;
    }
}

//在此处初始化
GlEngine *GlEngine::glEngine = NULL;
Lock GlEngine::mLock;
bool GlEngine::isInitComplete = false;

GlEngine *GlEngine::getGlEngine() {
    if (glEngine == NULL) {
        mLock.lock();
        if (glEngine == NULL) {
            glEngine = new GlEngine();
            GlEngine::isInitComplete = true;
        }
        mLock.unlock();
    }

    return GlEngine::glEngine;
}

extern "C" {
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1resize_1opengl(
        JNIEnv *env, jclass clazz, jint width, jint height);
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1step_1opengl(
        JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1init_1opengl(
        JNIEnv *env, jclass clazz);

};

// onSurfaceChanged
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1resize_1opengl(
        JNIEnv *env, jclass clazz, jint width, jint height) {
    ALOGI("native_1resize_1opengl IN");
    // setupGraphicsTest(width,height);
    ALOGI("native_1resize_1opengl OUT");
}
// onDrawFrame
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1step_1opengl(
        JNIEnv *env, jclass clazz) {
    ALOGI("native_1step_1opengl IN");
    // renderFrameTest();
    GlEngine::getGlEngine()->drawFrame();
    ALOGI("native_1step_1opengl OUT");

}

// onSurfaceCreated
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1init_1opengl(
        JNIEnv *env, jclass clazz) {

    ALOGI("native_1init_1opengl IN");
    GlEngine::getGlEngine()->setupGraphics();

    ALOGI("native_1init_1opengl OUT");

}