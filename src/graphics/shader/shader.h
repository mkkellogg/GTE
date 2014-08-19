
#ifndef _SHADER_H_
#define _SHADER_H_

#include "graphics/vertexattrbuffer.h"

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
    protected: 

    public :

    Shader(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~Shader();
    virtual bool Load() = 0;
    virtual int GetVariableLocation(const char *varName) const = 0;
    virtual void SendBufferToShader(int loc, VertexAttrBuffer * buffer) = 0;
};

#endif
