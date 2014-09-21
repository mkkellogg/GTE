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

	class MaterialImportDescriptor
	{
		public:

		bool invertVCoords;
		std::map<ShaderMaterialCharacteristic, int> uvMapping;

		bool UVMappingHasKey(ShaderMaterialCharacteristic key)
		{
			if(uvMapping.find(key) != uvMapping.end())
			{
				return true;
			}

			return false;
		}

		MaterialImportDescriptor()
		{
			invertVCoords = true;
		}
	};

	void RecursiveProcessModelScene(const aiScene *sc, const aiNode* nd, float scale, SceneObject * parent, Matrix4x4 * currentTransform, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor *>& materialImportDescriptors);
	SceneObject * ProcessModelScene(const std::string& modelPath, const aiScene* scene, float importScale);
	bool ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor *>& materialImportDescriptors);
	static LongMask GetImportFlags(const aiMaterial * mat);
	Mesh3D * ConvertAssimpMesh(const aiMesh* mesh, Material * material, MaterialImportDescriptor * materialImportDescriptor);
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
