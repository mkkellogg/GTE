
#ifndef _SHADER_H_
#define _SHADER_H_

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
};

#endif
