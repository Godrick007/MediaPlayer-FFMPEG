//
// Created by Godrick Crown on 2020/12/9.
//

#include "GLESRenderer.h"
#include <EGL/egl.h>
#include <cstdlib>


static const char VERTEX_SHADER[] = "#version 100\n"
                                    "attribute vec4 av_Position;\n"
                                    "attribute vec2 af_Position;\n"
                                    "varying vec2 v_texPosition;\n"
                                    "void main(){\n"
                                    "    v_texPosition = af_Position;\n"
                                    "    gl_Position = av_Position;\n"
                                    "}";

static const char FRAGMENT_SHADER_TEST[] = "#version 100\n"
                                           "precision mediump float;\n"
                                           "uniform vec4 v_texPosition;\n"
                                           "void main(){\n"
                                           "     gl_FragColor = v_texPosition;\n"
                                           "}";


static const char FRAGMENT_SHADER[] = "#version 100\n"
                                      "precision mediump float;\n"
                                      "varying vec2 v_texPosition;\n"
                                      "uniform sampler2D sampler_y;\n"
                                      "uniform sampler2D sampler_u;\n"
                                      "uniform sampler2D sampler_v;\n"
                                      "void main() {\n"
                                      "    float y,u,v;\n"
                                      "    y = texture2D(sampler_y,v_texPosition).r;\n"
                                      "    u = texture2D(sampler_u,v_texPosition).r- 0.5;\n"
                                      "    v = texture2D(sampler_v,v_texPosition).r- 0.5;\n"
                                      "    vec3 rgb;\n"
                                      "    rgb.r = y + 1.403 * v;\n"
                                      "    rgb.g = y - 0.344 * u - 0.714 * v;\n"
                                      "    rgb.b = y + 1.770 * u;\n"
                                      "    gl_FragColor = vec4(rgb,1);\n"
                                      "}";

class RendererES2 : public Renderer {

public:
    bool init();

private :


};

Renderer *createES2Renderer() {
    RendererES2 *renderer = new RendererES2;
    if (!renderer->init()) {
        return nullptr;
        delete renderer;
    }
    return renderer;
}

Renderer::Renderer() : mEglContext(eglGetCurrentContext()) {
    eglUtil = new EGLUtil;
}

Renderer::~Renderer() {
    if (mEglContext != eglGetCurrentContext()) {
        return;
    }
    LOGD("OpenGLES2.0", "~Renderer() has called");
    glDeleteProgram(mProgram);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
}


void Renderer::render() {
    draw();
}

bool RendererES2::init() {

//    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (eglUtil == nullptr) {
        return false;
    }

    mProgram = eglUtil->createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram) {
        return false;
    }

//    av_Position = glGetAttribLocation(mProgram, "av_Position");
//    af_Position = glGetUniformLocation(mProgram, "v_texPosition");
//
//    glGenBuffers(1, &mVertexPositionBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

//    if (true) {
//        return true;
//    }

    av_Position = glGetAttribLocation(mProgram, "av_Position");
    af_Position = glGetAttribLocation(mProgram, "af_Position");
    sampler_y = glGetUniformLocation(mProgram, "sampler_y");
    sampler_u = glGetUniformLocation(mProgram, "sampler_u");
    sampler_v = glGetUniformLocation(mProgram, "sampler_v");

//    glGenBuffers(1, &mVertexPositionBuffer);
////    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
////    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
////
////    glGenBuffers(1, &mFragmentPositionBuffer);
////    glBindBuffer(GL_ARRAY_BUFFER, mFragmentPositionBuffer);
////    glBufferData(GL_ARRAY_BUFFER, sizeof(textureData), textureData, GL_STATIC_DRAW);

    glGenTextures(3, textureId_yuv);
    for (int i = 0; i < 3; i++) {
        glBindTexture(GL_TEXTURE_2D, textureId_yuv[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    return true;
}

void Renderer::setYUVData(int width, int height, void *y, void *u, void *v) {


//    LOGI("java_count", "set yuv data %ld", count++);
    this->width_yuv = width;
    this->height_yuv = height;
    this->y = y;
    this->u = u;
    this->v = v;
    drawing = false;
}


void Renderer::resize(int width, int height) {
    width_surface = width;
    height_surface = height;
    calcTextureData();
    glViewport(0, 0, width, height);
}

void Renderer::setYUVSize(int width, int height) {
    width_yuv = width;
    height_yuv = height;
    calcTextureData();
}

void Renderer::calcTextureData() {


    if (width_yuv > 0 && height_yuv > 0 && width_surface > 0 && height_surface > 0) {


        if (width_yuv * 1.0f / height_yuv >
            width_surface * 1.0f / height_surface) {//视频数据比例宽于surface，压缩高度

            float y = width_surface * 1.0f / width_yuv * height_yuv / height_surface;

            vertexData[0] = -1;
            vertexData[1] = -y;
            vertexData[2] = 1;
            vertexData[3] = -y;
            vertexData[4] = -1;
            vertexData[5] = y;
            vertexData[6] = 1;
            vertexData[7] = y;

        } else if (width_yuv * 1.0f / height_yuv < width_surface * 1.0f /
                                                   height_surface) { //视频数据比例窄于surface，压缩宽度


            float x = height_surface * 1.0f / height_yuv * width_yuv / width_surface;


            vertexData[0] = -x;
            vertexData[1] = -1;
            vertexData[2] = x;
            vertexData[3] = -1;
            vertexData[4] = -x;
            vertexData[5] = 1;
            vertexData[6] = x;
            vertexData[7] = 1;

        } else {
            vertexData[0] = -1;
            vertexData[1] = -1;
            vertexData[2] = 1;
            vertexData[3] = -1;
            vertexData[4] = -1;
            vertexData[5] = 1;
            vertexData[6] = 1;
            vertexData[7] = 1;
//             {
//                    -1.0f, -1.0f,
//                    1.0f, -1.0f,
//                    -1.0f, 1.0f,
//                    1.0f, 1.0f
//            };
        }


    }

}

//RendererES2::~RendererES2() {
//    if (eglGetCurrentContext() != mEglContext) {
//        return;
//    }
//
//    glDeleteBuffers(1, &mVertexPositionBuffer);
//    glDeleteBuffers(1, &mFragmentPositionBuffer);
//    glDeleteProgram(mProgram);
//
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClearColor(0, 0, 0, 1);
//
//}

void Renderer::draw() {


    if (width_yuv > 0 && height_yuv > 0 && y != nullptr && v != nullptr && v != nullptr) {

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);
        glUseProgram(mProgram);

        glEnableVertexAttribArray(av_Position);
        glVertexAttribPointer(av_Position, 2, GL_FLOAT, GL_FALSE, 8,
                              (const void *) vertexData);


        glEnableVertexAttribArray(af_Position);
        glVertexAttribPointer(af_Position, 2, GL_FLOAT, GL_TRUE, 8,
                              textureData);

//        OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。
//        通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节。
//        但是这个图片是jpg并且宽高不是4的倍数，所以出现了问题。
//        通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId_yuv[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_yuv, height_yuv, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, y);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureId_yuv[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, u);


        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureId_yuv[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, v);

        glUniform1i(sampler_y, 0);
        glUniform1i(sampler_u, 1);
        glUniform1i(sampler_v, 2);

//        free(y);
//        free(u);
//        free(v);

//        y = nullptr;
//        u = nullptr;
//        v = nullptr;

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}


