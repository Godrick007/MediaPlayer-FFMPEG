//
// Created by Godrick Crown on 2020/12/24.
//

#ifndef MEDIAPLAYER_FFMPEG_EGLUTIL_H
#define MEDIAPLAYER_FFMPEG_EGLUTIL_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "LogUtil.h"

extern "C" {
#include <stdlib.h>
};

class EGLUtil {

public:

    void printGLString(const char *name, GLenum s);

    bool checkGLError(const char *funcName);

    GLuint createShader(GLenum shaderType, const char *src);

    GLuint createProgram(const char *vertexSrc, const char *fragmentSrc);

};


#endif //MEDIAPLAYER_FFMPEG_EGLUTIL_H
