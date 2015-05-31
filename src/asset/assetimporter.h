#ifndef _GTE_ASSET_IMPORTER_H_
#define _GTE_ASSET_IMPORTER_H_

#include "object/engineobjectmanager.h"
#include "object/enginetypes.h"
#include "global/global.h"
#include <string>

namespace GTE
{
	//forward declarations
	class ShaderSourceLoader;

	enum class AssetImporterBoolProperty
	{
		PreserveFBXPivots = 0,
		_Count = 1,
	};

	class AssetImporter
	{
		ShaderSourceLoader * shaderSourceLoader;

		bool boolProperties[(UInt32)AssetImporterBoolProperty::_Count];

	public:

		AssetImporter();
		~AssetImporter();

		SceneObjectRef LoadModelDirect(const std::string& filePath) const;
		SceneObjectRef LoadModelDirect(const std::string& filePath, Real importScale, bool castShadows, bool receiveShadows) const;
		AnimationRef LoadAnimation(const std::string& filePath, bool addLoopPadding) const;
		void LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource);

		void SetBoolProperty(AssetImporterBoolProperty, bool value);
		bool GetBoolProperty(AssetImporterBoolProperty prop) const;
	};
}

#endif
