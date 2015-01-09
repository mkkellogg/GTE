#ifndef _GTE_SHADER_SOURCE_H_
#define _GTE_SHADER_SOURCE_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

class ShaderSource
{
    bool loaded;
    char * sourceString;
    std::string sourcePath;
    char * ReadShaderSource(const char *fn);

    public :

    ShaderSource(const std::string& filePath);
    ~ShaderSource();
    char * GetSourceString();
    bool Load();
    bool IsLoaded();
};

#endif
