#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

//forward declarations
class Matrix4x4;
class EngineObjectManager;
class SceneObject;
class SubMesh3D;
class Material;
class UV2Array;
class Skeleton;
class VertexBoneMap;
namespace Assimp { class Importer; }

#include "object/sceneobjectcomponent.h"
#include "object/shadermanager.h"
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "base/longmask.h"

#include <memory>
#include <string>
#include <vector>

class ModelImporter
{
	friend EngineObjectManager;

	protected :

	enum class SceneTraverseOrder
	{
		PreOrder=0
	};

	class MeshSpecificMaterialDescriptor
	{
		public:

		int vertexColorsIndex;
		LongMask shaderProperties;
		MaterialRef material;

		MeshSpecificMaterialDescriptor()
		{
			vertexColorsIndex = -1;
			shaderProperties = 0L;
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

	static const std::string AssimpPathDelimiter;
	Assimp::Importer * importer;

	bool InitImporter();
	const aiScene * LoadAIScene(const std::string& filePath);

	void RecursiveProcessModelScene(const aiScene& scene, const aiNode& nd, float scale, SceneObjectRef parent,   std::vector<MaterialImportDescriptor>& materialImportDescriptors, SkeletonRef skeleton, std::vector<SceneObjectRef>& createdSceneObjects) const;
	SceneObjectRef ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale) const;
	bool ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const;
	static void GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene);
	SubMesh3DRef ConvertAssimpMesh(const aiMesh& mesh, unsigned int meshIndex, MaterialImportDescriptor& materialImportDescriptor) const;

	SkeletonRef LoadSkeleton(const aiScene& scene) const;
	VertexBoneMap * ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh) const;
	void AddBoneMappings(SkeletonRef skeleton, const aiMesh& mesh, unsigned int& currentBoneIndex, VertexBoneMap& vertexIndexBoneMap) const;
	unsigned CountBones(const aiScene& scene) const;
	bool CreateAndMapNodeHierarchy(SkeletonRef skeleton, const aiScene& scene) const;
	AnimationRef LoadAnimation (aiAnimation& animation, SkeletonRef skeleton) const;

	void TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<bool(const aiNode&)> callback) const;
	void PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<bool(const aiNode&)> callback) const;

	static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
	static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
	static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property);
	static std::string GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);

	public:

	ModelImporter();
	~ModelImporter();

	SceneObjectRef LoadModelDirect(const std::string& filePath, float importScale);
	AnimationRef LoadAnimation(const std::string& filePath);

};

#endif
