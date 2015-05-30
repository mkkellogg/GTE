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

namespace GTE
{
	AssetImporter::AssetImporter()
	{
		// TODO: Make this choose the correct implementation based on platform.
		// For now we go with OpenGL by default
		shaderSourceLoader = new ShaderSourceLoaderGL();

		for (unsigned int i = 0; i < (unsigned int)AssetImporterBoolProperty::_Count; i++)
		{
			boolProperties[i] = false;
		}
	}

	AssetImporter::~AssetImporter()
	{
		SAFE_DELETE(shaderSourceLoader);
	}

	SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath) const
	{
		return LoadModelDirect(filePath, 1, true, true);
	}

	SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, float importScale, bool castShadows, bool receiveShadows) const
	{
		ModelImporter importer;
		return importer.LoadModelDirect(filePath, importScale, castShadows, receiveShadows, GetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots));
	}

	AnimationRef AssetImporter::LoadAnimation(const std::string& filePath, bool addLoopPadding) const
	{
		ModelImporter importer;
		return importer.LoadAnimation(filePath, addLoopPadding, GetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots));
	}

	void AssetImporter::LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource)
	{
		ASSERT(shaderSourceLoader != NULL, "AssetImporter::LoadBuildInShaderSource -> shaderSourceLoader is NULL.");
		shaderSourceLoader->LoadShaderSouce(name, shaderSource);
	}

	void AssetImporter::SetBoolProperty(AssetImporterBoolProperty prop, bool value)
	{
		boolProperties[(unsigned int)prop] = value;
	}

	bool AssetImporter::GetBoolProperty(AssetImporterBoolProperty prop) const
	{
		return boolProperties[(unsigned int)prop];
	}
}

