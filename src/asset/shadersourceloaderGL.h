#ifndef _GTE_SHADER_SOURCE_LOADER_GL_H_
#define _GTE_SHADER_SOURCE_LOADER_GL_H_

//forward declarations
class ShaderSource;

#include <string>
#include "shadersourceloader.h"

class ShaderSourceLoaderGL : public ShaderSourceLoader
{
	friend class AssetImporter;

	protected:

	ShaderSourceLoaderGL();
	~ShaderSourceLoaderGL();

    public :

    void LoadShaderSouce(const std::string name, ShaderSource& shaderSource) const;
};

#endif
