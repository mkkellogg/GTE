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

#include "object/sceneobjectcomponent.h"
#include "object/shaderorganizer.h"
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "base/longmask.h"
#include "global/global.h"

#include <memory>
#include <string>
#include <vector>

namespace GTE
{
	//forward declarations
	class Matrix4x4;
	class EngineObjectManager;
	class SceneObject;
	class SubMesh3D;
	class Material;
	class UV2Array;
	class Skeleton;
	class VertexBoneMap;
	
	enum ModelImporterErrorCodes
	{
		ModelFileNotFound = 1,
		ModelFileLoadFailed = 2,
		AssimpTextureNotFound = 3,
		TextureFileNotFound = 4,
		TextureFileLoadFailed = 5,
		MaterialImportFailure = 6,
		MaterialShaderMatchFailure = 7,
		MaterialShaderVariableMatchFailure = 8,
		ProcessMaterialsFailed = 9
	};

	class ModelImporter
	{
		friend class AssetImporter;

	protected:

		enum class TextureType
		{
			Diffuse = 0,
			Specular = 1,
			BumpMap = 2,
			_None = 3
		};

		enum class SceneTraverseOrder
		{
			PreOrder = 0
		};

		class MeshSpecificMaterialDescriptor
		{
		public:

			Int32 vertexColorsIndex;
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
				if (uvMapping.find(key) != uvMapping.end())return true;
				return false;
			}
		};

		class MaterialImportDescriptor
		{
		public:

			std::map<int, MeshSpecificMaterialDescriptor> meshSpecificProperties;

			bool UsedByMesh(Int32 index)
			{
				if (meshSpecificProperties.find(index) != meshSpecificProperties.end())return true;
				return false;
			}
		};

		Assimp::Importer * importer;

		ModelImporter();
		~ModelImporter();

		void InitImporter();
		const aiScene * LoadAIScene(const std::string& filePath, bool preserveFBXPivots);

		void RecursiveProcessModelScene(const aiScene& scene, const aiNode& nd, Real scale, SceneObjectRef parent,
			std::vector<MaterialImportDescriptor>& materialImportDescriptors, SkeletonRef skeleton,
			std::vector<SceneObjectRef>& createdSceneObjects, bool castShadows, bool receiveShadows) const;
		SceneObjectRef ProcessModelScene(const std::string& modelPath, const aiScene& scene, Real importScale, bool castShadows, bool receiveShadows) const;
		bool ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const;
		TextureRef LoadAITexture(aiMaterial& material, aiTextureType textureType, const std::string& modelPath) const;
		bool SetupMeshSpecificMaterialWithTexture(const aiMaterial& assimpMaterial, const TextureType textureType, TextureRef texture,
			UInt32 meshIndex, MaterialImportDescriptor& materialImportDesc) const;
		static void GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene);
		SubMesh3DRef ConvertAssimpMesh(UInt32 meshIndex, const aiScene& scene, MaterialImportDescriptor& materialImportDescriptor, bool invert) const;
		void SetupVertexBoneMapForRenderer(const aiScene& scene, SkeletonRef skeleton, SkinnedMesh3DRendererRef target, bool reverseVertexOrder) const;

		SkeletonRef LoadSkeleton(const aiScene& scene) const;
		VertexBoneMap * ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh, bool reverseVertexOrder) const;
		void AddMeshBoneMappingsToSkeleton(SkeletonRef skeleton, const aiMesh& mesh, UInt32& currentBoneIndex) const;
		void SetupVertexBoneMapMappingsFromAIMesh(SkeletonRef skeleton, const aiMesh& mesh, VertexBoneMap& vertexIndexBoneMap) const;
		unsigned CountBones(const aiScene& scene) const;
		bool CreateAndMapNodeHierarchy(SkeletonRef skeleton, const aiScene& scene) const;
		AnimationRef LoadAnimation(aiAnimation& animation, bool addLoopPadding) const;

		void TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<bool(const aiNode&)> callback) const;
		void PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<bool(const aiNode&)> callback) const;

		static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
		static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
		static StandardUniform MapTextureTypeToUniform(TextureType textureType);
		static StandardAttribute MapTextureTypeToAttribute(TextureType textureType);
		static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property);
		static std::string GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);
		static std::string GetBuiltinVariableNameForTextureType(TextureType textureType);
		static TextureType ConvertAITextureKeyToTextureType(Int32 aiTextureKey);
		static Int32 ConvertTextureTypeToAITextureKey(TextureType textureType);

		static bool HasOddReflections(Matrix4x4& mat);

	public:

		SceneObjectRef LoadModelDirect(const std::string& modelPath, Real importScale, bool castShadows, bool receiveShadows, bool preserveFBXPivots);
		AnimationRef LoadAnimation(const std::string& filePath, bool addLoopPadding, bool preserveFBXPivots);

	};
}

#endif
