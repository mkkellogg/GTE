#ifndef _GTE_SHADER_SOURCE_LOADER_H_
#define _GTE_SHADER_SOURCE_LOADER_H_

//forward declarations
class ShaderSource;

#include <string>

class ShaderSourceLoader
{
	friend class AssetImporter;

	protected:

	ShaderSourceLoader();
	virtual ~ShaderSourceLoader();

    public :

    virtual void LoadShaderSouce(const std::string name, ShaderSource& shaderSource) const = 0;
};

#endif
