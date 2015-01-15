#ifndef _GTE_SHADER_SOURCE_H_
#define _GTE_SHADER_SOURCE_H_

#include <string>

enum class ShaderSourceType
{
	File=0,
	String=1
};

enum ShaderSourceError
{
	LoadError = 0
};

class ShaderSource
{
	std::string name;
	ShaderSourceType sourceType;
    bool loaded;
    bool initialized;

    std::string vertexSourceString;
    std::string vertexSourcePath;
    std::string fragmentSourceString;
    std::string fragmentSourcePath;

    char * ReadShaderSource(const char *fn);
    void CopyToThis(const ShaderSource& source);

    public :

    ShaderSource();
    ShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& name);
    ShaderSource(const ShaderSource& source);
    ShaderSource& operator= (const ShaderSource& source);
    ~ShaderSource();
    void Init(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& name);
    const std::string& GetVertexSourceString() const;
    const std::string& GetFragmentSourceString() const;
    const std::string& GetName() const;
    bool Load();
    bool IsLoaded();
};

#endif
