#ifndef _GTE_ASSET_IMPORTER_H_
#define _GTE_ASSET_IMPORTER_H_

//forward declarations
class ShaderSourceLoader;

#include "object/engineobjectmanager.h"
#include "object/enginetypes.h"
#include <string>

class AssetImporter
{
	ShaderSourceLoader * shaderSourceLoader;

	public:

	AssetImporter();
	~AssetImporter();

	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale) const;
	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale, bool castShadows, bool receiveShadows) const;
	AnimationRef LoadAnimation(const std::string& filePath) const;
	void LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource);
};

#endif
