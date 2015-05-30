
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "shadersourceloaderGL.h"
#include "graphics/shader/shadersource.h"
#include "filesys/filesystem.h"
#include "global/global.h"
#include "global/constants.h"

namespace GTE
{
	ShaderSourceLoaderGL::ShaderSourceLoaderGL()
	{

	}

	ShaderSourceLoaderGL::~ShaderSourceLoaderGL()
	{

	}

	void ShaderSourceLoaderGL::LoadShaderSouce(const std::string name, ShaderSource& shaderSource) const
	{
		std::string vertexSource;
		std::string fragmentSource;

		FileSystem * fileSystem = FileSystem::Instance();
		std::string builtinPath = fileSystem->FixupPathForLocalFilesystem(Constants::BuiltinShaderPathOpenGL);

		vertexSource = fileSystem->ConcatenatePaths(builtinPath, std::string(name + std::string(".vertex.shader")));
		fragmentSource = fileSystem->ConcatenatePaths(builtinPath, std::string(name + std::string(".fragment.shader")));
		shaderSource.Init(vertexSource, fragmentSource, ShaderSourceType::File, name);
	}
}

