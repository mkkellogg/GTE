/*
 * Class: ModelImporter
 *
 * Author: Mark Kellogg
 *
 * This class utilizes the functions and data structures in Assimp to
 * load models from disk and convert them into equivalent engine-native
 * scene object hierarchies with meshes and materials.
 */

#ifndef _GTE_MODEL_IMPORTER_H_
#define _GTE_MODEL_IMPORTER_H_

//forward declarations
class Matrix4x4;
class EngineObjectManager;
class SceneObject;
class SubMesh3D;
class Material;
class UV2Array;
class Skeleton;
class VertexBoneMap;

// forward declaration for Assimp
namespace Assimp { class Importer; }

#include "object/sceneobjectcomponent.h"
#include "object/shaderorganizer.h"
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "base/longmask.h"

#include <memory>
#include <string>
#include <vector>

enum ModelImporterErrorCodes
{
	ModelFileNotFound = 1,
	ModelFileLoadFailed = 2,
	AssimpTextureNotFound = 3,
	TextureFileNotFound = 4,
	TextureFileLoadFailed = 5,
	MaterialImportFailure = 6,
	MaterialTextureLoadFailure = 7,
	MaterialShaderMatchFailure = 8,
	MaterialShaderVariableMatchFailure = 9


};

class ModelImporter
{
	friend class AssetImporter;

	protected :

	enum class TextureType
	{
		Diffuse = 0,
		Specular = 1,
		BumpMap = 2,
		_None = 3
	};

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
		bool invertVCoords;
		std::map<TextureType, int> uvMapping;

		MeshSpecificMaterialDescriptor()
		{
			vertexColorsIndex = -1;
			shaderProperties = 0L;
			invertVCoords = true;
		}

		bool UVMappingHasKey(TextureType key)
		{
			if(uvMapping.find(key) != uvMapping.end())return true;
			return false;
		}
	};

	class MaterialImportDescriptor
	{
		public:

		std::map<int,MeshSpecificMaterialDescriptor> meshSpecificProperties;

		bool UsedByMesh(int index)
		{
			if(meshSpecificProperties.find(index) != meshSpecificProperties.end())return true;
			return false;
		}
	};

	Assimp::Importer * importer;

	ModelImporter();
	~ModelImporter();

	bool InitImporter();
	const aiScene * LoadAIScene(const std::string& filePath);

	void RecursiveProcessModelScene(const aiScene& scene, const aiNode& nd, float scale, SceneObjectRef parent,
									std::vector<MaterialImportDescriptor>& materialImportDescriptors, SkeletonRef skeleton,
									std::vector<SceneObjectRef>& createdSceneObjects,  bool castShadows, bool receiveShadows) const;
	SceneObjectRef ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale, bool castShadows, bool receiveShadows) const;
	bool ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const;
	TextureRef LoadAITexture(aiMaterial& material, aiTextureType textureType, const std::string& modelPath) const;
	bool SetupMeshSpecificMaterialWithTexture(const aiMaterial& assimpMaterial, const TextureType textureType, TextureRef texture,
			 	 	 	 	 	 	 	 	  unsigned int meshIndex, MaterialImportDescriptor& materialImportDesc) const;
	static void GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene);
	SubMesh3DRef ConvertAssimpMesh(const aiMesh& mesh, unsigned int meshIndex, MaterialImportDescriptor& materialImportDescriptor) const;
	void SetupVertexBoneMapForRenderer(const aiScene& scene, SkeletonRef skeleton, SkinnedMesh3DRendererRef target) const;

	SkeletonRef LoadSkeleton(const aiScene& scene) const;
	VertexBoneMap * ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh) const;
	void AddMeshBoneMappingsToSkeleton(SkeletonRef skeleton, const aiMesh& mesh, unsigned int& currentBoneIndex) const;
	void SetupVertexBoneMapMappingsFromAIMesh(SkeletonRef skeleton, const aiMesh& mesh, VertexBoneMap& vertexIndexBoneMap) const;
	unsigned CountBones(const aiScene& scene) const;
	bool CreateAndMapNodeHierarchy(SkeletonRef skeleton, const aiScene& scene) const;
	AnimationRef LoadAnimation (aiAnimation& animation) const;

	void TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<bool(const aiNode&)> callback) const;
	void PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<bool(const aiNode&)> callback) const;

	static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
	static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
	static StandardUniform MapTextureTypeToUniform(TextureType textureType);
	static StandardAttribute MapTextureTypeToAttribute(TextureType textureType);
	static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property);
	static std::string GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);
	static std::string GetBuiltinVariableNameForTextureType(TextureType textureType);
	static TextureType ConvertAITextureKeyToTextureType(int aiTextureKey);
	static int ConvertTextureTypeToAITextureKey(TextureType textureType);

	public:

	SceneObjectRef LoadModelDirect(const std::string& modelPath, float importScale, bool castShadows, bool receiveShadows);
	AnimationRef LoadAnimation(const std::string& filePath);

};

#endif
