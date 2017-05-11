//
// Created by Administrator on 2017/4/11.
//
#define LOG_NDEBUG 0
#define TAG "YTX-GLThread-JNI"
#include "ytxplayer/ALog-priv.h"
#include <ytxplayer/GlslFilter.h>
#include "ytxplayer/GLThread.h"

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

auto gVertexShader =
        "attribute vec4 vPosition;\n"
                "void main() {\n"
                "  gl_Position = vPosition;\n"
                "}\n";

auto gFragmentShader =
        "precision mediump float;\n"
                "void main() {\n"
                "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
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


GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
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

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    ALOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        ALOGE("Could not create program.");
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    ALOGI("glGetAttribLocation(\"vPosition\") = %d\n",
         gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");
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


GLThread::GLThread(VideoStateInfo *mVideoStateInfo) {

    this->mVideoStateInfo = mVideoStateInfo;
    mPicture=NULL;
    mTexturePicture=NULL;
    RGBABuffer = NULL;

}

GLThread::~GLThread() {

    if(RGBABuffer != NULL){
        free(RGBABuffer);
        RGBABuffer = NULL;
    }

    if(mPicture != NULL){
        free(mPicture);
        mPicture = NULL;
    }

    if(mTexturePicture != NULL){
        free(mTexturePicture);
        mTexturePicture = NULL;
    }
}

void GLThread::handleRun(void *ptr)
{
    if (!prepare()) {
        ALOGI("Couldn't prepare AudioRefreshController\n");
        return;
    }
    refresh();

}

void GLThread::refresh() {
    AVMessage msg;
    while (mRunning) {
        if (mVideoStateInfo->messageQueueGL->get(&msg, true) < 0) {
            mRunning = false;
        } else {
            if (!process(&msg)) {
                mRunning = false;
            }
        }
    }

}

bool GLThread::process(AVMessage *msg) {
    bool ret = true;
//
//    if (msg->what == GL_MSG_CANCEL) {
//        deInitEGL();
//        ret = false;
//    }

    switch (msg->what) {
        case GL_MSG_RENDERER:
            VMessageData *vData = (VMessageData *) msg->priv_data;
            drawGL(glslFilter,vData);
            break;
//        case GL_MSG_DECODED_FIRST_FRAME:
//
//            break;
    }

    return ret;

}

bool GLThread::prepare() {

    initEGL(mVideoStateInfo->mVideoWidth,mVideoStateInfo->mVideoHeight);
//    glslFilter = new GlslFilter();
//    glslFilter->initial();
//    glslFilter->buildTextures();
    setupGraphics(surfaceWidth, surfaceHeight);
    return true;
}

void GLThread::SnapshotBmpRGB(GLvoid *pData, int width, int height, char *filename)
{

    BITMAPFILEHEADER fileHead={0};
    BITMAPINFOHEADER infoHead={0};
    int biBitCount = 24;
    long dataSize = (width*biBitCount+31)/32*4*height;
    FILE *fp =  fopen(filename, "wb");

    // 位图第一部分，文件信息
    fileHead.cfType[0] = 0x42;
    fileHead.cfType[1] = 0x4d;
    fileHead.cfSize = dataSize + 54;
    fileHead.cfReserved = 0;
    fileHead.cfoffBits = 54;
    // 位图第二部分，数据信息

    infoHead.ciSize[0] = 40;
    infoHead.ciWidth = width;
    infoHead.ciHeight = height;
    infoHead.ciPlanes[0] = 1;
    infoHead.ciBitCount = 24;
    infoHead.ciCompress[3] = 0;
    infoHead.ciSizeImage[3] = 0;
    infoHead.ciXPelsPerMeter[3] = 0;
    infoHead.ciYPelsPerMeter[3] = 0;
    infoHead.ciClrUsed[3] = 0;
    infoHead.ciClrImportant[3] = 0;

    fwrite(&fileHead,1, sizeof(BITMAPFILEHEADER),fp);
    fwrite(&infoHead,1, sizeof(BITMAPINFOHEADER),fp);
    fwrite(pData,1,dataSize,fp);
    fclose(fp);

}

void GLThread::SnapshotBmpRGBA(GLvoid * pData, int width, int height,  char * filename)
{

    BITMAPFILEHEADER fileHead={0};
    BITMAPINFOHEADER infoHead={0};
    int biBitCount = 32;
    long dataSize = (width*biBitCount+31)/32*4*height;
    FILE *fp =  fopen(filename, "wb");

    // 位图第一部分，文件信息
    fileHead.cfType[0] = 0x42;
    fileHead.cfType[1] = 0x4d;
    fileHead.cfSize = dataSize + 54;
    fileHead.cfReserved = 0;
    fileHead.cfoffBits = 54;
    // 位图第二部分，数据信息

    infoHead.ciSize[0] = 40;
    infoHead.ciWidth = width;
    infoHead.ciHeight = height;
    infoHead.ciPlanes[0] = 1;
    infoHead.ciBitCount = biBitCount;
    infoHead.ciCompress[3] = 0;
    infoHead.ciSizeImage[3] = 0;
    infoHead.ciXPelsPerMeter[3] = 0;
    infoHead.ciYPelsPerMeter[3] = 0;
    infoHead.ciClrUsed[3] = 0;
    infoHead.ciClrImportant[3] = 0;

    fwrite(&fileHead,1, sizeof(BITMAPFILEHEADER),fp);
    fwrite(&infoHead,1, sizeof(BITMAPINFOHEADER),fp);
    fwrite(pData,1,dataSize,fp);
    fclose(fp);

}

void GLThread::initEGL(int width, int height) {

    // EGL config attributes
    const EGLint confAttr[] =
            {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,// very important!
                    EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
                    EGL_RED_SIZE,   8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE,  8,
                    EGL_ALPHA_SIZE, 8,// if you need the alpha channel
                    EGL_DEPTH_SIZE, 8,// if you need the depth buffer
                    EGL_STENCIL_SIZE,8,
                    EGL_NONE
            };
    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,// very important!
            EGL_NONE
    };
    // surface attributes
    // the surface size is set to the input frame size
    const EGLint surfaceAttr[] = {
            EGL_WIDTH,width,
            EGL_HEIGHT,height,
            EGL_NONE
    };
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;

    eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisp == EGL_NO_DISPLAY)
    {
        //Unable to open connection to local windowing system
        ALOGI("Unable to open connection to local windowing system");
    }
    if(!eglInitialize(eglDisp, &eglMajVers, &eglMinVers))
    {
        // Unable to initialize EGL. Handle and recover
        ALOGI("Unable to initialize EGL");
    }
    ALOGI("EGL init with version %d.%d", eglMajVers, eglMinVers);
    // choose the first config, i.e. best config
    if(!eglChooseConfig(eglDisp, confAttr, &eglConf, 1, &numConfigs))
    {
        ALOGI("some config is wrong");
    }
    else
    {
        ALOGI("all configs is OK");
    }
    // create a pixelbuffer surface
    //eglSurface = eglCreatePbufferSurface(eglDisp, eglConf, surfaceAttr);
    //EGLNativeWindowType window = android_createDisplaySurface();
    ALOGI("ytxhaoo eglCreateWindowSurface mVideoStateInfo->window=%#x",mVideoStateInfo->window);
    eglSurface = eglCreateWindowSurface(eglDisp, eglConf, mVideoStateInfo->window, NULL);
    ALOGI("ytxhaoo eglCreateWindowSurface eglSurface=%#x",eglSurface);
    if(eglSurface == EGL_NO_SURFACE)
    {
       // ALOGI("ytxhaoo eglCreateWindowSurface eglGetError=%#x",eglGetError());
        EGLint err = eglGetError();
        ALOGI("ytxhaoo eglCreateWindowSurface err=%#x",err);
        switch(err)
        {
            case EGL_BAD_ALLOC:
                // Not enough resources available. Handle and recover
                ALOGE("Not enough resources available");
                break;
            case EGL_BAD_CONFIG:
                // Verify that provided EGLConfig is valid
                ALOGE("provided EGLConfig is invalid");
                break;
            case EGL_BAD_PARAMETER:
                // Verify that the EGL_WIDTH and EGL_HEIGHT are
                // non-negative values
                ALOGE("provided EGL_WIDTH and EGL_HEIGHT is invalid");
                break;
            case EGL_BAD_MATCH:
                // Check window and EGLConfig attributes to determine
                // compatibility and pbuffer-texture parameters
                ALOGE("Check window and EGLConfig attributes");
                break;
            case EGL_BAD_SURFACE:
                ALOGE("Check surface and EGLConfig attributes");
                break;
            case EGL_BAD_ATTRIBUTE:
                ALOGE("Check EGLConfig attributes bad");
                break;
        }
    }

    eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);
    if(eglCtx == EGL_NO_CONTEXT)
    {
        EGLint error = eglGetError();
        if(error == EGL_BAD_CONFIG)
        {
            // Handle error and recover
            ALOGI("EGL_BAD_CONFIG");
        }
    }

    eglQuerySurface(eglDisp, eglSurface, EGL_WIDTH, &surfaceWidth);
    eglQuerySurface(eglDisp, eglSurface, EGL_HEIGHT, &surfaceHeight);
    ALOGI("ytxhao eglQuerySurface w=%d h=%d",surfaceWidth,surfaceHeight);
    if(!eglMakeCurrent(eglDisp, eglSurface, eglSurface, eglCtx))
    {
        ALOGI("MakeCurrent failed");
    }
    ALOGI("initialize success!");

}

void GLThread::drawGL(GlslFilter *filter,VMessageData *vData ) {



//    filter->process(vData);
//    filter->drawFrame();

    // clear screen
//    glDisable(GL_DITHER);
//    glDisable(GL_SCISSOR_TEST);
//    glClearColor(0,0,1,1);
//    glClear(GL_COLOR_BUFFER_BIT);
    //调用eglSwapBuffers会去触发queuebuffer，dequeuebuffer，
    //queuebuffer将画好的buffer交给surfaceflinger处理，
    //dequeuebuffer新创建一个buffer用来画图
    renderFrame();
    eglSwapBuffers(eglDisp, eglSurface);

}

void GLThread::deInitEGL() {


    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglSurface);
    eglTerminate(eglDisp);

    eglDisp = EGL_NO_DISPLAY;
    eglSurface = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;
}

void GLThread::stop() {

    int ret = -1;
    mVideoStateInfo->messageQueueGL->abort();
    mRunning = false;
    if ((ret = wait()) != 0) {
        ALOGE("Couldn't cancel GLThread: %i\n", ret);
        return;
    }


}