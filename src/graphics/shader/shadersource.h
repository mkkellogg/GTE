#ifndef _SHADER_SOURCE_H_
#define _SHADER_SOURCE_H_

#include <GL/glew.h>
#include <GL/glut.h>

class ShaderSource
{
    bool loaded;
    char * sourceString;
    const char * sourcePath;
    char * ReadShaderSource(const char *fn);

    public :

    ShaderSource(const char *filePath);
    ~ShaderSource();
    char * GetSourceString();
    bool Load();
    bool IsLoaded();
};

#endif
