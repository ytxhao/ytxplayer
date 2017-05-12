//
// Created by Administrator on 2017/4/7.
//

#define LOG_NDEBUG 0
#define TAG "YTX-GlslFilter-JNI"
#include <ytxplayer/ALog-priv.h>
#include <ytxplayer/GlslFilter.h>

static const char gVertexShader[]=
                "attribute vec4 a_position;\n"
                "attribute vec2 a_texcoord;\n"
                "varying vec2 tc;\n"
                "void main() {\n"
                "gl_Position = a_position;\n"
                "tc = a_texcoord;\n"
                "}\n";

static const char gFragmentShader[]=
                "precision mediump float;\n"
                "uniform sampler2D tex_y;\n"
                "uniform sampler2D tex_u;\n"
                "uniform sampler2D tex_v;\n"
                "uniform sampler2D tex_png;\n"
                "varying vec2 tc;\n"
                "void main() {\n"
                "vec4 c = vec4((texture2D(tex_y, tc).r - 16./255.) * 1.164);\n"
                "vec4 U = vec4(texture2D(tex_u, tc).r - 128./255.);\n"
                "vec4 V = vec4(texture2D(tex_v, tc).r - 128./255.);\n"
                "c += V * vec4(1.596, -0.813, 0, 0);\n"
                "c += U * vec4(0, -0.392, 2.017, 0);\n"
                "c.a = 1.0;\n"
                "vec4 color_png = texture2D(tex_png, tc);\n"
                "gl_FragColor = c + color_png;\n"
                "}\n";



GlslFilter::GlslFilter() {
    isInitialed = false;


    yTextureId = 1025;
    uTextureId = 1025;
    vTextureId = 1025;
    pngTextureId = 1025;

    shaderProgram = 0;
    yHandle = -1;
    uHandle = -1;
    vHandle = -1;
    pngHandle = -1;

    img = NULL;

    vertexShader = 0;
    pixelShader = 0;

    videoWidth = 0;
    videoHeight = 0;
    mScreenWidth = 0;
    mScreenHeight = 0;

}

GlslFilter::~GlslFilter() {

}


const char * GlslFilter::getVertexShaderString() {


    return gVertexShader;
}

const char * GlslFilter::getFragmentShaderString() {

    return gFragmentShader;

}
void GlslFilter::initial() {

    if(isInitialed){
        return;
    }

    isInitialed = true;

    //fp_yuv = fopen("/storage/emulated/0/ytx.yuv","wb+");
    createProgram(getVertexShaderString(), getFragmentShaderString());
    glUseProgram(shaderProgram);
    checkGlError("glUseProgram");

}


void GlslFilter::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    ALOGI("GL %s = %s\n", name, v);
}

void GlslFilter::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        ALOGI("after %s() glError (0x%x)\n", op, error);
    }
}


GLuint GlslFilter::loadShader(GLenum shaderType, const char *pSource) {

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

GLuint GlslFilter::createProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLint ret;
    shaderProgram = 0;
    vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        ALOGE("create vertexShader error !");
        return 0;
    }

    pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        ALOGE("create pixelShader error !");
        return 0;
    }

    shaderProgram = glCreateProgram();
    if (shaderProgram) {
        glAttachShader(shaderProgram, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(shaderProgram, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(shaderProgram);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(shaderProgram, bufLength, NULL, buf);
                    ALOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }
    }


    ret = glGetAttribLocation(shaderProgram, "a_position");
    if (ret == -1) {
        ALOGE("setupGraphics error glGetAttribLocation(gProgram, \"a_position\") =%d\n", ret);
    }
    checkGlError("glGetAttribLocation");
    posCoordHandle = (GLuint) ret;


    ret = glGetAttribLocation(shaderProgram, "a_texcoord");
    if (ret == -1) {
        ALOGE("setupGraphics error GetAttribLocation(gProgram, \"a_texcoord\") = %d\n", ret);
    }
    checkGlError("glGetAttribLocation");
    texCoordHandle = (GLuint) ret;

    /*
     * get uniform location for y/u/v, we pass data through these uniforms
     */

    yHandle = glGetUniformLocation(shaderProgram, "tex_y");
    checkGlError("glGetUniformLocation tex_y");
    ALOGI("GLProgram yHandle = %d\n", yHandle);
    if (yHandle == -1) {
        ALOGE("Could not get uniform location for tex_y");
    }

    uHandle = glGetUniformLocation(shaderProgram, "tex_u");
    checkGlError("glGetUniformLocation tex_u");
    ALOGI("GLProgram uHandle = %d\n", uHandle);
    if (uHandle == -1) {
        ALOGE("Could not get uniform location for tex_u");
    }

    vHandle = glGetUniformLocation(shaderProgram, "tex_v");
    checkGlError("glGetUniformLocation tex_v");
    ALOGI("GLProgram vHandle = %d\n", vHandle);
    if (vHandle == -1) {
        ALOGE("Could not get uniform location for tex_v");
    }


    pngHandle = glGetUniformLocation(shaderProgram, "tex_png");
    checkGlError("glGetUniformLocation pngHandle");
    ALOGI("GLProgram pngHandle = %d\n", pngHandle);
    if (pngHandle == -1) {
        ALOGE("Could not get uniform location for pngHandle");
    }

    return shaderProgram;

}

void GlslFilter::setScreenWidth(int mScreenWidth) {
    this->mScreenWidth = mScreenWidth;
}

void GlslFilter::setScreenHeight(int mScreenHeight) {
    this->mScreenHeight = mScreenHeight;
}

void GlslFilter::renderBackground() {
    glClearColor(0.10588f, 0.109804f, 0.12157f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GlslFilter::process(VMessageData *vData ) {
   // ALOGI("ytxhao test gl y=%#x u=%#x videoWidth=%d decodeHeight=%d",vData->y,vData->u,vData->videoWidth,vData->videoHeight);
    addRendererFrame(vData);
}

void GlslFilter::drawFrame() {


    mLock.lock();
    if (plane[0] != NULL && plane[1] != NULL && plane[2] != NULL && videoWidth != 0 &&
        videoHeight != 0) {

//        fwrite(plane[0],1,videoWidth*videoHeight,fp_yuv);    //Y
//        fwrite(plane[1],1,videoWidth*videoHeight/4,fp_yuv);  //U
//        fwrite(plane[2],1,videoWidth*videoHeight/4,fp_yuv);  //V

        setAspectRatio();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glVertexAttribPointer(posCoordHandle, 2, GL_FLOAT, false, 8, posVertices);
        checkGlError("glVertexAttribPointer posCoordHandle");
        glEnableVertexAttribArray(posCoordHandle);

        glVertexAttribPointer(texCoordHandle, 2, GL_FLOAT, false, 8, texVertices);
        checkGlError("glVertexAttribPointer texCoordHandle");
        glEnableVertexAttribArray(texCoordHandle);

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


        if (img != NULL) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, pngTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, img->buffer);
            checkGlError("glTexImage2D");
            glUniform1i(pngHandle, 3);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFinish();

        glDisableVertexAttribArray(posCoordHandle);
        glDisableVertexAttribArray(texCoordHandle);

    }
    mLock.unlock();

}

void GlslFilter::setAspectRatio(){

    float f1 = (float) mScreenHeight / mScreenWidth;
    float f2 = (float) videoHeight / videoWidth;
    float widthScale = 0.0;
    float heightScale = 0.0;

    if (f1 == f2) {

    } else if (f1 < f2) {
        widthScale = f1 / f2;
        posVertices[0] = -widthScale;
        posVertices[1] = -1.0f;
        posVertices[2] = widthScale;
        posVertices[3] = -1.0f;
        posVertices[4] = -widthScale;
        posVertices[5] = 1.0f;
        posVertices[6] = widthScale;
        posVertices[7] = 1.0f;

    } else if (f1 > f2) {
        heightScale = f2 / f1;
        posVertices[0] = -1.0f;
        posVertices[1] = -heightScale;
        posVertices[2] = 1.0f;
        posVertices[3] = -heightScale;
        posVertices[4] = -1.0f;
        posVertices[5] = heightScale;
        posVertices[6] = 1.0f;
        posVertices[7] = heightScale;
    }
}
void GlslFilter::addRendererFrame(VMessageData *vData){

    mLock.lock();
    addRendererFrameInit(vData);

    if (vData->img != NULL) {
        memcpy(img->buffer, img->buffer, (size_t) (img->stride * img->height));
    } else {
        memset(img->buffer, 0, (size_t) (img->stride * img->height));
    }

    memcpy(plane[0], vData->y, (size_t) (vData->videoWidth * vData->videoHeight));
    memcpy(plane[1], vData->u, (size_t) (vData->videoWidth * vData->videoHeight) / 4);
    memcpy(plane[2], vData->v, (size_t) (vData->videoWidth * vData->videoHeight) / 4);
    mLock.unlock();
}

void GlslFilter::addRendererFrameInit(VMessageData *vData){
    if ((this->videoWidth == 0 && this->videoHeight == 0 && vData->videoWidth != 0)
        || this->videoWidth != vData->videoWidth || this->videoHeight != vData->videoHeight) {
        this->videoWidth = vData->videoWidth;
        this->videoHeight = vData->videoHeight;
        ALOGI("addRendererFrameInit isAddRendererFrameInit");
        if (plane[0] == NULL) {
            plane[0] = (char *) malloc(sizeof(char) * videoWidth * videoHeight);
            plane[1] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);
            plane[2] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);
        } else {
            char *plane_tmp[3];
            plane_tmp[0] = plane[0];
            plane_tmp[1] = plane[1];
            plane_tmp[2] = plane[2];
            plane[0] = (char *) malloc(sizeof(char) * videoWidth * videoHeight);
            plane[1] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);
            plane[2] = (char *) malloc(sizeof(char) * videoWidth * videoHeight / 4);

            free(plane_tmp[0]);
            free(plane_tmp[1]);
            free(plane_tmp[2]);
        }

        if (img == NULL) {
            img = gen_image(videoWidth, videoHeight);
        } else {
            image_t *img_tmp = img;
            img = gen_image(videoWidth, videoHeight);
            free(img_tmp);
        }

    }
}
image_t *GlslFilter::gen_image(int width, int height) {

    image_t *img = (image_t *) malloc(sizeof(image_t));
    img->width = width;
    img->height = height;
    img->stride = width * 3;
    img->buffer = (unsigned char *) calloc(1, height * width * 3);
    memset(img->buffer, 0, img->stride * img->height);
    return img;
}

GLuint GlslFilter::createTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    checkGlError("glGenTextures");
    return texture;
}

void GlslFilter::setViewPort(int mSurfaceWidth, int mSurfaceHeight) {
    glViewport(0, 0, mSurfaceWidth, mSurfaceHeight);
    checkGlError("glViewport");
}

void GlslFilter::buildTextures() {

    if (yTextureId == 1025) {
        glGenTextures(1, &yTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        checkGlError("glGenTextures");
        ALOGI("buildTextures yTextureId=%d\n", yTextureId);

    } else {
        glDeleteTextures(1, &yTextureId);
        checkGlError("glDeleteTextures");
        glGenTextures(1, &yTextureId);
        checkGlError("glGenTextures");
    }

    glBindTexture(GL_TEXTURE_2D, yTextureId);
    checkGlError("glBindTexture");

    //  设置纹理参数,缩小过滤,邻近像素差值(临近像素插值。该方法质量较差)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //设置纹理参数,放大过滤,线性插值(线性插值。使用距离当前渲染像素中心最近的4个纹素加权平均值)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    // building texture for U data

    if (uTextureId == 1025) {
        glGenTextures(1, &uTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        checkGlError("glGenTextures");
        ALOGI("buildTextures uTextureId=%d\n", uTextureId);
    } else {
        glDeleteTextures(1, &uTextureId);
        checkGlError("glDeleteTextures");
        glGenTextures(1, &uTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        checkGlError("glGenTextures");
        ALOGI("buildTextures uTextureId=%d\n", uTextureId);
    }


    glBindTexture(GL_TEXTURE_2D, uTextureId);
    checkGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    if (vTextureId == 1025) {
        glGenTextures(1, &vTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        checkGlError("glGenTextures");
        ALOGI("buildTextures vTextureId=%d\n", vTextureId);
    } else {
        glDeleteTextures(1, &vTextureId);
        checkGlError("glDeleteTextures");

        glGenTextures(1, &vTextureId);
        checkGlError("glGenTextures");
        ALOGI("buildTextures vTextureId=%d\n", vTextureId);
    }


    glBindTexture(GL_TEXTURE_2D, vTextureId);
    checkGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    if (pngTextureId == 1025) {
        glGenTextures(1, &pngTextureId);  //参数1:用来生成纹理的数量. 参数2:存储纹理索引的第一个元素指针
        checkGlError("glGenTextures");
        ALOGI("buildTextures pngTextureId=%d\n", pngTextureId);
    } else {
        glDeleteTextures(1, &pngTextureId);
        checkGlError("glDeleteTextures");

        glGenTextures(1, &pngTextureId);
        checkGlError("glGenTextures");
        ALOGI("buildTextures pngTextureId=%d\n", pngTextureId);
    }


    glBindTexture(GL_TEXTURE_2D, pngTextureId);
    checkGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

