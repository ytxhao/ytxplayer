#define  TAG  "gl2jni"

#include "ALog-priv.h"
#include "gl_engine.h"

GlEngine::GlEngine() {

//    yTextureId = 255;
//    uTextureId = 255;
//    vTextureId = 255;
    yHandle = -1;
    uHandle = -1;
    vHandle = -1;
    isAddRendererFrameInit = false;
    isSetupGraphics = false;
    isDrawFrameInit = false;
    isFrameRendererFinish = false;


}

GlEngine::~GlEngine() {

    int i = 0;

    for (i = 0; i < 3; i++) {
        if (plane[i] != NULL) {
            free(plane[i]);
            plane[i] = NULL;
        }
    }

    if (glEngine != NULL) {
        delete glEngine;
    }
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
    GLint ret = -1;
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

    ret = glGetAttribLocation(gProgram, "vPosition");
    if (ret == -1) {
        ALOGE("setupGraphics error glGetAttribLocation(gProgram, \"vPosition\") =%d\n", ret);
    }
    checkGlError("glGetAttribLocation");
    gvPositionHandle = (GLuint) ret;


    ret = glGetAttribLocation(gProgram, "a_texCoord");
    if (ret == -1) {
        ALOGE("setupGraphics error GetAttribLocation(gProgram, \"a_texCoord\") = %d\n", ret);
    }
    checkGlError("glGetAttribLocation");
    gCoordHandle = (GLuint) ret;

    /*
     * get uniform location for y/u/v, we pass data through these uniforms
     */

    yHandle = glGetUniformLocation(gProgram, "tex_y");
    checkGlError("glGetUniformLocation tex_y");
    ALOGI("GLProgram yHandle = %d\n", yHandle);

    if (yHandle == -1) {
        ALOGE("Could not get uniform location for tex_y");
    }
    uHandle = glGetUniformLocation(gProgram, "tex_u");
    checkGlError("glGetUniformLocation tex_u");
    ALOGI("GLProgram uHandle = %d\n", uHandle);

    if (uHandle == -1) {
        ALOGE("Could not get uniform location for tex_u");
    }
    vHandle = glGetUniformLocation(gProgram, "tex_v");
    checkGlError("glGetUniformLocation tex_v");
    ALOGI("GLProgram vHandle = %d\n", vHandle);
    if (vHandle == -1) {
        ALOGE("Could not get uniform location for tex_v");
    }

    glUseProgram(program);
    checkGlError("glUseProgram");

    isSetupGraphics = true;
    return true;
}

void GlEngine::setVideoWidthAndHeight(int videoWidth, int videoHeight) {
    this->videoWidth = videoWidth;
    this->videoHeight = videoHeight;
}

void GlEngine::buildTextures() {
    // building texture for Y data

    glGenTextures(1, &yTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
    checkGlError("glGenTextures");
    ALOGI("buildTextures yTextureId=%d\n", yTextureId);

    glBindTexture(GL_TEXTURE_2D, yTextureId);
    checkGlError("glBindTexture");

    //  设置纹理参数,缩小过滤,邻近像素差值(临近像素插值。该方法质量较差)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //设置纹理参数,放大过滤,线性插值(线性插值。使用距离当前渲染像素中心最近的4个纹素加权平均值)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // building texture for U data

    glGenTextures(1, &uTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
    checkGlError("glGenTextures");
    ALOGI("buildTextures uTextureId=%d\n", uTextureId);

    glBindTexture(GL_TEXTURE_2D, uTextureId);
    checkGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glGenTextures(1, &vTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
    checkGlError("glGenTextures");
    ALOGI("buildTextures vTextureId=%d\n", vTextureId);

    glBindTexture(GL_TEXTURE_2D, vTextureId);
    checkGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


}

void GlEngine::drawFrameInit(int videoWidth, int videoHeight) {
    if (!isDrawFrameInit) {
        isDrawFrameInit = true;
//        glViewport(0, 0, videoWidth, videoHeight);
//        checkGlError("glViewport");
        buildTextures();
    }
}

/**
 * render the frame
 * the YUV data will be converted to RGB by shader.
 */
void GlEngine::drawFrame() {
    if (plane[0] != NULL && plane[1] != NULL && plane[2] != NULL && videoWidth != 0 &&
        videoHeight != 0) {

        drawFrameInit(videoWidth, videoHeight);
        setAspectRatio();


        ALOGI("drawFrame videoWidth=%d videoHeight=%d\n", videoWidth, videoHeight);


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, false, 8, vertice_buffer);
        checkGlError("glVertexAttribPointer mPositionHandle");
        glEnableVertexAttribArray(gvPositionHandle);

        glVertexAttribPointer(gCoordHandle, 2, GL_FLOAT, false, 8, coord_buffer);
        checkGlError("glVertexAttribPointer maTextureHandle");
        glEnableVertexAttribArray(gCoordHandle);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, yTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth, videoHeight, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, plane[0]);
        checkGlError("glTexImage2D");
        glUniform1i(yHandle, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth / 2, videoHeight / 2, 0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, plane[1]);
        checkGlError("glTexImage2D");
        glUniform1i(uHandle, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, vTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoWidth / 2, videoHeight / 2, 0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, plane[2]);
        checkGlError("glTexImage2D");
        glUniform1i(vHandle, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFinish();

        glDisableVertexAttribArray(gvPositionHandle);
        glDisableVertexAttribArray(gCoordHandle);
    }

}

void GlEngine::signalRendererFinish() {

    mRendererLock.lock();
    isFrameRendererFinish = true;
    mRendererLock.condSignal();
    mRendererLock.unlock();
}

void GlEngine::waitRendererFinish() {
    mRendererLock.lock();
    while (!isFrameRendererFinish) {
        mRendererLock.condWait();
    }
    isFrameRendererFinish = false;
    mRendererLock.unlock();
}

void GlEngine::addRendererFrame(char *y, char *u, char *v, int videoWidth, int videoHeight) {
    addRendererFrameInit(videoWidth, videoHeight);
    memcpy(plane[0], y, (size_t) (videoWidth * videoHeight));
    memcpy(plane[1], u, (size_t) (videoWidth * videoHeight) / 4);
    memcpy(plane[2], v, (size_t) (videoWidth * videoHeight) / 4);
}

void GlEngine::addRendererFrameInit(int videoWidth, int videoHeight) {
    if (!isAddRendererFrameInit) {
        isAddRendererFrameInit = true;
        this->videoWidth = videoWidth;
        this->videoHeight = videoHeight;
        plane[0] = (char *) malloc(sizeof(char) * videoWidth * videoHeight);
        plane[1] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);
        plane[2] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);
    }
}

void GlEngine::setAspectRatio() {
    float f1 = (float) mScreenHeight / mScreenWidth;
    float f2 = (float) videoHeight / videoWidth;
    float widthScale = 0.0;
    float heightScale = 0.0;
    ALOGI("setAspectRatio mScreenHeight=%d mScreenWidth=%d f1=%f\n", mScreenHeight,mScreenWidth,f1);
    ALOGI("setAspectRatio videoHeight=%d videoWidth=%d f2=%f\n", videoHeight,videoWidth,f2);


    if (f1 == f2) {

    } else if (f1 < f2) {
        widthScale = f1 / f2;
        ALOGI("setAspectRatio widthScale=%f\n", widthScale);
        vertice_buffer[0] = -widthScale;
        vertice_buffer[1] = -1.0f;
        vertice_buffer[2] = widthScale;
        vertice_buffer[3] = -1.0f;
        vertice_buffer[4] = -widthScale;
        vertice_buffer[5] = 1.0f;
        vertice_buffer[6] = widthScale;
        vertice_buffer[7] = 1.0f;
        
    } else if(f1 > f2){
        heightScale = f2 / f1;
        ALOGI("setAspectRatio heightScale=%f\n", heightScale);
        vertice_buffer[0] = -1.0f;
        vertice_buffer[1] = -heightScale;
        vertice_buffer[2] = 1.0f;
        vertice_buffer[3] = -heightScale;
        vertice_buffer[4] = -1.0f;
        vertice_buffer[5] = heightScale;
        vertice_buffer[6] = 1.0f;
        vertice_buffer[7] = heightScale;
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
            GlEngine::glSetEngineInitComplete(true);
        }
        mLock.unlock();
    }

    return GlEngine::glEngine;
}


void GlEngine::notifyRenderer() {
    //调用回调通知渲染视频
    if(notifyRendererCallback!=NULL){
        notifyRendererCallback();
    }

}

void GlEngine::setScreenWidth(int mScreenWidth){
    this->mScreenWidth = mScreenWidth;
}

void GlEngine::setScreenHeight(int mScreenHeight){
    this->mScreenHeight = mScreenHeight;
}

int GlEngine::getScreenWidth(){
    return  this->mScreenWidth ;
}

int GlEngine::getScreenHeight(){
    return  this->mScreenHeight;
}

void GlEngine::setViewPort(int mSurfaceWidth, int mSurfaceHeight) {
    glViewport(0, 0, mSurfaceWidth, mSurfaceHeight);
    checkGlError("glViewport");
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

    GlEngine::getGlEngine()->setScreenHeight(height);
    GlEngine::getGlEngine()->setScreenWidth(width);
    GlEngine::getGlEngine()->setViewPort(width,height);

    ALOGI("native_1resize_1opengl OUT");
}
// onDrawFrame
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1step_1opengl(
        JNIEnv *env, jclass clazz) {
    ALOGI("native_1step_1opengl IN");
    GlEngine::getGlEngine()->drawFrame();
  //  GlEngine::getGlEngine()->signalRendererFinish();
    ALOGI("native_1step_1opengl OUT");
}

// onSurfaceCreated
JNIEXPORT void JNICALL Java_com_ytx_ican_media_player_gl2jni_GL2JNILib_native_1init_1opengl(
        JNIEnv *env, jclass clazz) {

    ALOGI("native_1init_1opengl IN");
    if (!GlEngine::getGlEngine()->isSetupGraphics) {
        GlEngine::getGlEngine()->setupGraphics();
    }
    ALOGI("native_1init_1opengl OUT");

}