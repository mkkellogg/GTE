#ifndef _ASSET_IMPORTER_H_
#define _ASSET_IMPORTER_H_

//forward declarations
class SceneObject;

#include "object/engineobjectmanager.h"
#include <string>

class AssetImporter
{
	friend EngineObjectManager;

	protected :

	public:

	AssetImporter();
	~AssetImporter();

	SceneObject * LoadModelDirect(const std::string& filePath, float importScale);
};

#endif
