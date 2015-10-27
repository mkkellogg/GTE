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
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	AssetImporter::AssetImporter()
	{
		// TODO: Make this choose the correct implementation based on platform.
		// For now we go with OpenGL by default
		shaderSourceLoader = new(std::nothrow) ShaderSourceLoaderGL();
		ASSERT(shaderSourceLoader != nullptr, "AssetImporter::LoadBuildInShaderSource -> shaderSourceLoader is null.");

		for (UInt32 i = 0; i < (UInt32)AssetImporterBoolProperty::_Count; i++)
		{
			BoolProperties[i] = false;
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

	SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, Real importScale, Bool castShadows, Bool receiveShadows) const
	{
		ModelImporter importer;
		return importer.LoadModelDirect(filePath, importScale, castShadows, receiveShadows, GetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots));
	}

	AnimationRef AssetImporter::LoadAnimation(const std::string& filePath, Bool addLoopPadding) const
	{
		ModelImporter importer;
		return importer.LoadAnimation(filePath, addLoopPadding, GetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots));
	}

	void AssetImporter::LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource)
	{
		ASSERT(shaderSourceLoader != nullptr, "AssetImporter::LoadBuildInShaderSource -> shaderSourceLoader is null.");
		shaderSourceLoader->LoadShaderSource(name, shaderSource);
	}

	void AssetImporter::SetBoolProperty(AssetImporterBoolProperty prop, Bool value)
	{
		BoolProperties[(UInt32)prop] = value;
	}

	Bool AssetImporter::GetBoolProperty(AssetImporterBoolProperty prop) const
	{
		return BoolProperties[(UInt32)prop];
	}
}

