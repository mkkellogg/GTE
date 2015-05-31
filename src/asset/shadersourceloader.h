#ifndef _GTE_SHADER_SOURCE_LOADER_H_
#define _GTE_SHADER_SOURCE_LOADER_H_

#include "global/global.h"
#include <string>

namespace GTE
{
	//forward declarations
	class ShaderSource;

	class ShaderSourceLoader
	{
		friend class AssetImporter;

	protected:

		ShaderSourceLoader();
		virtual ~ShaderSourceLoader();

	public:

		virtual void LoadShaderSouce(const std::string name, ShaderSource& shaderSource) const = 0;
	};
}

#endif
