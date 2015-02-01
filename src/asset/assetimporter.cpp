#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "assetimporter.h"
#include "shadersourceloaderGL.h"
#include "object/enginetypes.h"
#include "modelimporter.h"
#include "global/global.h"
#include "debug/gtedebug.h"

AssetImporter::AssetImporter()
{
	// TODO: Make this choose the correct implementation based on platform.
	// For now we go with OpenGL by default
	shaderSourceLoader = new ShaderSourceLoaderGL();
}

AssetImporter::~AssetImporter()
{
	SAFE_DELETE(shaderSourceLoader);
}

SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, float importScale) const
{
	return LoadModelDirect(filePath, importScale, true, true);
}

SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, float importScale, bool castShadows, bool receiveShadows) const
{
	ModelImporter importer;
	return importer.LoadModelDirect(filePath, importScale, castShadows, receiveShadows);
}

AnimationRef AssetImporter::LoadAnimation(const std::string& filePath, bool addLoopPadding) const
{
	ModelImporter importer;
	return importer.LoadAnimation(filePath, addLoopPadding);
}

void AssetImporter::LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource)
{
	ASSERT_RTRN(shaderSourceLoader != NULL, "AssetImporter::LoadBuildInShaderSource -> shaderSourceLoader is NULL.");
	shaderSourceLoader->LoadShaderSouce(name, shaderSource);
}


