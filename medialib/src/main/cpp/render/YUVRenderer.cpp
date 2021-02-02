//
// Created by Godrick Crown on 2021/1/22.
//

#include "YUVRenderer.h"

YUVRenderer::YUVRenderer() : eglContext(eglGetCurrentContext()) {
    eglUtil = new EGLUtil();
}

YUVRenderer::~YUVRenderer() {
    LOGD("OpenGLES2.0", "YUVRenderer() has called");
    glDeleteProgram(mProgramSW);
    glDeleteProgram(mProgramHW);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
}

bool YUVRenderer::init() {
    return initSW() && initHW();
}

bool YUVRenderer::initSW() {
    if (eglUtil == nullptr) {
        return false;
    }

    mProgramSW = eglUtil->createProgram(VERTEX_SHADER, FRAGMENT_SHADER_SW);
    if (!mProgramSW) {
        return false;
    }

    av_PositionSW = glGetAttribLocation(mProgramSW, "av_Position");
    af_PositionSW = glGetAttribLocation(mProgramSW, "af_Position");
    sampler_y = glGetUniformLocation(mProgramSW, "sampler_y");
    sampler_u = glGetUniformLocation(mProgramSW, "sampler_u");
    sampler_v = glGetUniformLocation(mProgramSW, "sampler_v");

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

bool YUVRenderer::initHW() {

    if(eglUtil == nullptr){
        return false;
    }

    mProgramHW = eglUtil->createProgram(VERTEX_SHADER,FRAGMENT_SHADER_HW);




    return true;
}

void YUVRenderer::setYUVData(int width, int height, void *y, void *u, void *v) {
    this->width_yuv = width;
    this->height_yuv = height;
    this->y = y;
    this->u = u;
    this->v = v;
}

void YUVRenderer::calcTextureData() {
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
                                                   height_surface) { //视频数据比例窄于surface，压缩宽
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
        }
    }

}

void YUVRenderer::surfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
    this->width_surface = width;
    this->height_surface = height;
    calcTextureData();
}

void YUVRenderer::setYUVSize(int width, int height) {
    this->width_yuv = width;
    this->height_yuv = height;
    calcTextureData();
}

void YUVRenderer::drawYUV() {

//    glClear(GL_COLOR_BUFFER_BIT);
//    glClearColor(1, 0, 0, 1);
//    glEnableVertexAttribArray(av_PositionSW);
//    glVertexAttribPointer(av_PositionSW, 2, GL_FLOAT, GL_FALSE, 8,
//                          (const void *) vertexData);
//    glEnableVertexAttribArray(af_PositionSW);
//    glVertexAttribPointer(af_PositionSW, 2, GL_FLOAT, GL_TRUE, 8,
//                          textureVertex);
//
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    return;

    if (width_yuv > 0 && height_yuv > 0 && y != nullptr && v != nullptr && v != nullptr) {

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);
        glUseProgram(mProgramSW);

        glEnableVertexAttribArray(av_PositionSW);
        glVertexAttribPointer(av_PositionSW, 2, GL_FLOAT, GL_FALSE, 8,
                              vertexData);


        glEnableVertexAttribArray(af_PositionSW);
        glVertexAttribPointer(af_PositionSW, 2, GL_FLOAT, GL_TRUE, 8,
                              textureVertex);

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
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    }

}


