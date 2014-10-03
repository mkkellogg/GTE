#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

//forward declarations
class Matrix4x4;
class EngineObjectManager;
class SceneObject;
class SubMesh3D;
class Material;
class UV2Array;

#include "object/sceneobjectcomponent.h"
#include "object/shadermanager.h"
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "base/longmask.h"
#include <string>
#include <vector>

class ModelImporter
{
	friend EngineObjectManager;

	protected :

	class MeshSpecificMaterialDescriptor
	{
		public:

		int vertexColorsIndex;
		LongMask shaderProperties;
		Material * material;

		MeshSpecificMaterialDescriptor()
		{
			vertexColorsIndex = -1;
			shaderProperties = 0L;
			material = NULL;
		}
	};

	class MaterialImportDescriptor
	{
		public:

		std::map<int,MeshSpecificMaterialDescriptor> meshSpecificProperties;

		bool invertVCoords;
		std::map<ShaderMaterialCharacteristic, int> uvMapping;

		bool UVMappingHasKey(ShaderMaterialCharacteristic key)
		{
			if(uvMapping.find(key) != uvMapping.end())return true;
			return false;
		}

		bool UsedByMesh(int index)
		{
			if(meshSpecificProperties.find(index) != meshSpecificProperties.end())return true;
			return false;
		}

		MaterialImportDescriptor()
		{
			invertVCoords = true;
		}
	};

	void RecursiveProcessModelScene(const aiScene& scene, const aiNode& nd, float scale, SceneObjectRef parent, Matrix4x4& currentTransform,  std::vector<MaterialImportDescriptor>& materialImportDescriptors);
	SceneObjectRef ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale);
	bool ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors);
	static void GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene);
	SubMesh3DRef ConvertAssimpMesh(const aiMesh& mesh, unsigned int meshIndex, MaterialImportDescriptor& materialImportDescriptor);
	static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
	static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
	static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property);
	static std::string GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);

	public:

	ModelImporter();
	~ModelImporter();

	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale);
};

#endif
