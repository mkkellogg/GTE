#ifndef _ASSET_IMPORTER_H_
#define _ASSET_IMPORTER_H_

#include "object/engineobjectmanager.h"
#include "object/enginetypes.h"
#include <string>

class AssetImporter
{
	friend EngineObjectManager;

	public:

	AssetImporter();
	~AssetImporter();

	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale) const;
	AnimationRef LoadAnimation(const std::string& filePath) const;
};

#endif
