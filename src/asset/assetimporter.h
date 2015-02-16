#ifndef _GTE_ASSET_IMPORTER_H_
#define _GTE_ASSET_IMPORTER_H_

//forward declarations
class ShaderSourceLoader;

#include "object/engineobjectmanager.h"
#include "object/enginetypes.h"
#include <string>

enum class AssetImporterBoolProperty
{
	PreserveFBXPivots = 0,
	_Count = 1,
};

class AssetImporter
{
	ShaderSourceLoader * shaderSourceLoader;

	bool boolProperties[(unsigned int)AssetImporterBoolProperty::_Count];

	public:

	AssetImporter();
	~AssetImporter();

	SceneObjectRef LoadModelDirect(const std::string& filePath) const;
	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale, bool castShadows, bool receiveShadows) const;
	AnimationRef LoadAnimation(const std::string& filePath, bool addLoopPadding) const;
	void LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource);

	void SetBoolProperty(AssetImporterBoolProperty, bool value);
	bool GetBoolProperty(AssetImporterBoolProperty prop) const;
};

#endif
