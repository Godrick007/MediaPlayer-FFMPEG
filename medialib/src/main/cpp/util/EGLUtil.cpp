//
// Created by Godrick Crown on 2020/12/24.
//

#include "EGLUtil.h"


void EGLUtil::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGD("MediaPlayer", "GL - %s: %s", name, v);
}

bool EGLUtil::checkGLError(const char *funcName) {
    GLuint err = glGetError();
    if (err == GL_NO_ERROR) {
        return true;
    }
    return false;
}

GLuint EGLUtil::createShader(GLenum shaderType, const char *src) {

    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGLError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, nullptr);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            if (LOG_DEBUG) {
                GLchar *infoLog = static_cast<GLchar *>(malloc(infoLogLen));
                if (infoLog) {
                    glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog);
                    LOGD("MediaPlayer", "Could not compile %s shader:\n%s\n",
                         shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
                    free(infoLog);
                }
            }
        }
    }
    return shader;
}


GLuint EGLUtil::createProgram(const char *vertexSrc, const char *fragmentSrc) {
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vertexShader = createShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vertexShader) {
        goto exit;
    }

    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fragmentShader) {
        goto exit;
    }

    program = glCreateProgram();
    if (!program) {
        checkGLError("glCreateProgram");
        goto exit;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        if (LOG_DEBUG) {
            GLint infoLogLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
            if (infoLogLen) {
                GLchar *infoLog = static_cast<GLchar *>(malloc(infoLogLen));
                if (infoLog) {
                    glGetProgramInfoLog(program, infoLogLen, nullptr, infoLog);
                    LOGD("MediaPlayer", "Could not link program : \n%s\n", infoLog);
                }
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }


    exit:
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;

}