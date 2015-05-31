#ifndef _GTE_SHADER_SOURCE_LOADER_GL_H_
#define _GTE_SHADER_SOURCE_LOADER_GL_H_

#include <string>
#include "shadersourceloader.h"
#include "global/global.h"

namespace GTE
{
	//forward declarations
	class ShaderSource;

	class ShaderSourceLoaderGL : public ShaderSourceLoader
	{
		friend class AssetImporter;

	protected:

		ShaderSourceLoaderGL();
		~ShaderSourceLoaderGL();

	public:

		void LoadShaderSouce(const std::string name, ShaderSource& shaderSource) const;
	};

}
#endif
