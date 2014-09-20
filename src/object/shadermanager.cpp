#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "shadermanager.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp" //OO version Header!
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "graphics/shader/shader.h"
#include "base/longmask.h"
#include "global/global.h"
#include "ui/debug.h"


ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{

}

LongMask ShaderManager::GetImportFlags(const aiMaterial * mtl)
{
	LongMask flags = LongMaskUtil::CreateLongMask();
	aiString path;
	aiColor4t<float> color;

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::DiffuseTextured);
	}

	/*if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::SpecularTextured);
	}*/

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::Bumped);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::DiffuseColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::SpecularColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::EmissiveColored);
	}

	return flags;
}

void ShaderManager::AddShader(LongMask properties, Shader * shader)
{
	NULL_CHECK_RTRN(shader,"ShaderManager::AddShader -> shader is NULL.");

	if(!shader->IsLoaded())
	{
		Debug::PrintError("ShaderManager::AddShader -> Tried to add unloaded shader.");
		return;
	}

	loadedShaders[properties] = shader;
}

Shader * ShaderManager::GetShader(LongMask flags)
{
	if(loadedShaders.find(flags) != loadedShaders.end())
	{
		return loadedShaders[flags];
	}

	return NULL;
}
