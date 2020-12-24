//
// Created by Godrick Crown on 2020/12/8.
//

#ifndef MEDIAPLAYER_FFMPEG_GLESRENDERER_H
#define MEDIAPLAYER_FFMPEG_GLESRENDERER_H


#include "../util/LogUtil.h"


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
    ANativeWindow *window = nullptr;
    int width_yuv;
    int height_yuv;

    int width_surface;
    int height_surface;

    void *y = nullptr;
    void *u = nullptr;
    void *v = nullptr;

    float vertexData[8];

public:

    void printGLString(const char *name, GLenum s);

    bool checkGLError(const char *funcName);

    GLuint createShader(GLenum shaderType, const char *src);

    GLuint createProgram(const char *vertexSrc, const char *fragmentSrc);


    virtual ~Renderer();

    void resize(int width, int height);

    void setYUVSize(int width, int height);

    void calcTextureData();

    void render();

    void setSurface(ANativeWindow *window);

    void setYUVData(int width, int height, void *y, void *u, void *v);


protected:
    Renderer();

    void draw();

    const EGLContext mEglContext;
    GLuint mProgram;
    GLuint mVertexPositionBuffer;
    GLuint mFragmentPositionBuffer;
    GLuint av_Position;
    GLuint af_Position;
//    GLuint textureId;
    GLuint sampler_y;
    GLuint sampler_u;
    GLuint sampler_v;
    GLuint textureId_yuv[3];


private:


};

extern Renderer *createES2Renderer();

extern Renderer *createES3Renderer();


#endif //MEDIAPLAYER_FFMPEG_GLESRENDERER_H
