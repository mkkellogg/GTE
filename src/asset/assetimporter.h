#ifndef _GTE_ASSET_IMPORTER_H_
#define _GTE_ASSET_IMPORTER_H_

#include <string>

#include "engine.h"
#include "object/engineobjectmanager.h"
#include "global/global.h"

namespace GTE {
    //forward declarations
    class ShaderSourceLoader;

    enum class AssetImporterBoolProperty {
        PreserveFBXPivots = 0,
        _Count = 1,
    };

    class AssetImporter {
        ShaderSourceLoader * shaderSourceLoader;

        Bool BoolProperties[(UInt32)AssetImporterBoolProperty::_Count];

    public:

        AssetImporter();
        ~AssetImporter();

        SceneObjectSharedPtr LoadModelDirect(const std::string& filePath) const;
        SceneObjectSharedPtr LoadModelDirect(const std::string& filePath, Real importScale, Bool castShadows, Bool receiveShadows) const;
        AnimationSharedPtr LoadAnimation(const std::string& filePath, Bool addLoopPadding) const;
        void LoadBuiltInShaderSource(const std::string name, ShaderSource& shaderSource);

        void SetBoolProperty(AssetImporterBoolProperty prop, Bool value);
        Bool GetBoolProperty(AssetImporterBoolProperty prop) const;
    };
}

#endif
