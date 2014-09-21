#ifndef _ASSET_IMPORTER_H_
#define _ASSET_IMPORTER_H_

//forward declarations
class Matrix4x4;
class EngineObjectManager;
class SceneObject;
class Mesh3D;
class Material;
class UV2Array;

#include "object/sceneobjectcomponent.h"
#include "object/shadermanager.h"
#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "base/longmask.h"
#include <string>
#include <vector>

class AssetImporter
{
	friend EngineObjectManager;

	protected :

	class TextureUVMap
	{
		public:

		std::map<ShaderMaterialCharacteristic, int> mapping;

		bool HasKey(ShaderMaterialCharacteristic key)
		{
			if(mapping.find(key) != mapping.end())
			{
				return true;
			}

			return false;
		}
	};

	void RecursiveProcessModelScene(const aiScene *sc, const aiNode* nd, float scale, SceneObject * parent, Matrix4x4 * currentTransform, std::vector<Material *>& materials, std::vector<TextureUVMap *>& textureUVMaps);
	SceneObject * ProcessModelScene(const std::string& modelPath, const aiScene* scene, float importScale);
	bool ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials, std::vector<TextureUVMap *>& textureUVMaps);
	static LongMask GetImportFlags(const aiMaterial * mat);
	Mesh3D * ConvertAssimpMesh(const aiMesh* mesh, Material * material, TextureUVMap * textureUVMap);
	static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
	static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
	static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(Mesh3D * mesh, ShaderMaterialCharacteristic property);
	static std::string GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);

	public:

	AssetImporter();
	~AssetImporter();

	SceneObject * LoadModel(const std::string& filePath, float importScale);


};

#endif
