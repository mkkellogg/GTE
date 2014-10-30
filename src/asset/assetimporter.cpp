#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "assetimporter.h"
#include <string>
#include "object/enginetypes.h"
#include "modelimporter.h"
#include "global/global.h"
#include "ui/debug.h"

AssetImporter::AssetImporter()
{

}

AssetImporter::~AssetImporter()
{

}

SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, float importScale) const
{
	ModelImporter importer;
	return importer.LoadModelDirect(filePath, importScale);
}

AnimationRef AssetImporter::LoadAnimation(const std::string& filePath) const
{
	ModelImporter importer;
	return importer.LoadAnimation(filePath);
}


