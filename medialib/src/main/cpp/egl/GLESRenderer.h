//
// Created by Godrick Crown on 2020/12/8.
//

#ifndef MEDIAPLAYER_FFMPEG_GLESRENDERER_H
#define MEDIAPLAYER_FFMPEG_GLESRENDERER_H


#include "../util/LogUtil.h"
#include "../util/EGLUtil.h"
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>



extern "C" {
#include <stdlib.h>
};


#include <GLES2/gl2.h>
#include <EGL/egl.h>

//extern
//
//extern
//
//extern
//
//extern

//float vertexData[] = {
//        -1.0f, -1.0f,
//        1.0f, -1.0f,
//        -1.0f, 1.0f,
//        1.0f, 1.0f
//};

const float textureData[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};


class Renderer {

public:
    Renderer();

    ANativeWindow *window = nullptr;
    int width_yuv;
    int height_yuv;

    int width_surface;
    int height_surface;

    EGLUtil *eglUtil = nullptr;

    bool drawing = false;

    void *y = nullptr;
    void *u = nullptr;
    void *v = nullptr;

    float vertexData[8];

    long count = 0;

public:


    virtual ~Renderer();

    void resize(int width, int height);

    void setYUVSize(int width, int height);

    void calcTextureData();

    virtual void render();

    void setSurface(ANativeWindow *window);

    void setYUVData(int width, int height, void *y, void *u, void *v);


protected:
    void draw();

    const EGLContext mEglContext;
    GLuint mProgramSW;
    GLuint mVertexPositionBuffer;
    GLuint mFragmentPositionBuffer;
    GLuint av_PositionSW;
    GLuint af_PositionSW;
//    GLuint textureId;
    GLuint sampler_y;
    GLuint sampler_u;
    GLuint sampler_v;
    GLuint textureId_yuv[3];

    GLuint mProgramHW;
    GLuint av_PositionHW;
    GLuint af_PositionHW;
    GLuint sTextureHW;
    GLuint textureId_HW;
    ASurfaceTexture *surfaceTexture;


private:


};

extern Renderer *createES2Renderer();

extern Renderer *createES3Renderer();


#endif //MEDIAPLAYER_FFMPEG_GLESRENDERER_H
