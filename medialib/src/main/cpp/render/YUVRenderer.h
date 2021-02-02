//
// Created by Godrick Crown on 2021/1/22.
//

#ifndef MEDIAPLAYER_FFMPEG_YUVRENDERER_H
#define MEDIAPLAYER_FFMPEG_YUVRENDERER_H

#include <EGL/egl.h>
#include "../util/EGLUtil.h"


static const char VERTEX_SHADER[] = "#version 100\n"
                                    "attribute vec4 av_Position;\n"
                                    "attribute vec2 af_Position;\n"
                                    "varying vec2 v_texPosition;\n"
                                    "void main(){\n"
                                    "    v_texPosition = af_Position;\n"
                                    "    gl_Position = av_Position;\n"
                                    "}";

static const char FRAGMENT_SHADER_SW[] = "#version 100\n"
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


static const char FRAGMENT_SHADER_HW[] = "#version 100\n"
                                         "#extension GL_OES_EGL_image_external:require\n"
                                         "precision mediump float;\n"
                                         "varying vec2 v_texPosition;\n"
                                         "uniform samplerExternalOES sTexture;\n"
                                         "\n"
                                         "void main(){\n"
                                         "    gl_FragColor = texture2D(sTexture,v_texPosition);\n"
                                         "}";

const float textureVertex[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};


enum RenderMode {
    RENDERMODE_WHEN_DIRTY,
    RENDERMODE_CONTINUOUSLY
};

class YUVRenderer {

public:
    RenderMode renderMode = RENDERMODE_CONTINUOUSLY;
    const EGLContext eglContext;
    EGLUtil *eglUtil = nullptr;

    int width_yuv;
    int height_yuv;

    int width_surface;
    int height_surface;

    void *y = nullptr;
    void *u = nullptr;
    void *v = nullptr;

    float vertexData[8];

    float vertexDL = 0;

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


public:
    YUVRenderer();

    ~YUVRenderer();

    bool init();

    int getRenderMode();

    void setYUVData(int width, int height, void *y, void *u, void *v);

    void surfaceChanged(int width, int height);

    void setYUVSize(int width, int height);

    void drawYUV();

private :
    bool initSW();

    bool initHW();

    void calcTextureData();

};


#endif //MEDIAPLAYER_FFMPEG_YUVRENDERER_H
