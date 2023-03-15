#pragma once

#include <map>
#include <string>

#include "glad/glad.h"

enum GLSLShaderType {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    TESS_CONTROL = 100,//GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION = 102,// GL_TESS_EVALUATION_SHADER,
    COMPUTE = 103//GL_COMPUTE_SHADER
};

class GLSLProgram {
    static std::string toString(GLSLShaderType type);

public:
    GLSLProgram();

    ~GLSLProgram();

public:
    bool build(const char *vs, const char *fs, const char *gs = NULL);

    bool build(const char *vs, const char *fs, const char *tc, const char *te);

    bool use();

    void clear();

public:
    void setUniform1i(const char *name, int val);

    void setUniform1f(const char *name, float val);

    void setUniform2f(const char *name, float x, float y);

    void setUniform2fv(const char *name, int count, const GLfloat *data);

    void setUniform3f(const char *name, float x, float y, float z);

    void setUniform4f(const char *name, float x, float y, float z, float w);

    void setUniform4fv(const char *name, const float *data);

    void setUniformMatrix3fv(const char *name, const GLfloat *data);

    void setUniformMatrix4fv(const char *name, const GLfloat *data);

private:
    bool compileShader(const char *file, GLSLShaderType ShaderType);

    bool link();

    bool compile(const char *vs, const char *fs);

    int getUniformLocation(const char *name);

private:
    GLuint program;
    std::map<std::string, int> uniformSets;
};
