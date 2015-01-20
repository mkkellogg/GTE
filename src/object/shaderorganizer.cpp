#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "shaderorganizer.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "graphics/shader/shader.h"
#include "base/longmask.h"
#include "global/global.h"
#include "debug/gtedebug.h"

ShaderOrganizer::ShaderOrganizer()
{

}

ShaderOrganizer::~ShaderOrganizer()
{

}

void ShaderOrganizer::AddShader(LongMask properties, ShaderRef shader)
{
	ASSERT_RTRN(shader.IsValid(),"ShaderManager::AddShader -> shader is NULL.");

	if(!shader->IsLoaded())
	{
		Debug::PrintError("ShaderManager::AddShader -> Tried to add unloaded shader.");
		return;
	}

	loadedShaders[properties] = shader;
}

// TODO: improve this function to move beyond exact matching of shader property flags.
// Ultimately we want it to return a shader that doesn't have any ADDITIONAL properties
// beyond what is specified by [flags], but it's ok to have less. However, EXACT
// matching is preferred for best results.
ShaderRef ShaderOrganizer::GetShader(LongMask flags)
{
	if(loadedShaders.find(flags) != loadedShaders.end())
	{
		return loadedShaders[flags];
	}

	return ShaderRef::Null();
}


