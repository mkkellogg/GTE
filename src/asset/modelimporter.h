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
			MaterialSharedPtr material;
			Bool invertVCoords;
			std::map<TextureType, int> uvMapping;

			MeshSpecificMaterialDescriptor()
			{
				vertexColorsIndex = -1;
				shaderProperties = 0L;
				invertVCoords = true;
			}

			Bool UVMappingHasKey(TextureType key)
			{
				if (uvMapping.find(key) != uvMapping.end())return true;
				return false;
			}
		};

		class MaterialImportDescriptor
		{
		public:

			std::map<int, MeshSpecificMaterialDescriptor> meshSpecificProperties;

			Bool UsedByMesh(Int32 index)
			{
				if (meshSpecificProperties.find(index) != meshSpecificProperties.end())return true;
				return false;
			}
		};

		Assimp::Importer * importer;

		ModelImporter();
		~ModelImporter();

		void InitImporter();
		const aiScene * LoadAIScene(const std::string& filePath, Bool preserveFBXPivots);

		void RecursiveProcessModelScene(const aiScene& scene, const aiNode& nd, Real scale, SceneObjectSharedPtr parent,
			std::vector<MaterialImportDescriptor>& materialImportDescriptors, SkeletonSharedPtr skeleton,
			std::vector<SceneObjectSharedPtr>& createdSceneObjects, Bool castShadows, Bool receiveShadows) const;
		SceneObjectSharedPtr ProcessModelScene(const std::string& modelPath, const aiScene& scene, Real importScale, Bool castShadows, Bool receiveShadows) const;
		Bool ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const;
		TextureSharedPtr LoadAITexture(aiMaterial& material, aiTextureType textureType, const std::string& modelPath) const;
		Bool SetupMeshSpecificMaterialWithTexture(const aiMaterial& assimpMaterial, const TextureType textureType, TextureSharedPtr texture,
			UInt32 meshIndex, MaterialImportDescriptor& materialImportDesc) const;
		static void GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene);
		SubMesh3DSharedPtr ConvertAssimpMesh(UInt32 meshIndex, const aiScene& scene, MaterialImportDescriptor& materialImportDescriptor, Bool invert) const;
		void SetupVertexBoneMapForRenderer(const aiScene& scene, SkeletonSharedPtr skeleton, SkinnedMesh3DRendererSharedPtr target, Bool reverseVertexOrder) const;

		SkeletonSharedPtr LoadSkeleton(const aiScene& scene) const;
		VertexBoneMap * ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh, Bool reverseVertexOrder) const;
		void AddMeshBoneMappingsToSkeleton(SkeletonSharedPtr skeleton, const aiMesh& mesh, UInt32& currentBoneIndex) const;
		void SetupVertexBoneMapMappingsFromAIMesh(SkeletonSharedConstPtr skeleton, const aiMesh& mesh, VertexBoneMap& vertexIndexBoneMap) const;
		unsigned CountBones(const aiScene& scene) const;
		Bool CreateAndMapNodeHierarchy(SkeletonSharedPtr skeleton, const aiScene& scene) const;
		AnimationSharedPtr LoadAnimation(aiAnimation& animation, Bool addLoopPadding) const;

		void TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<Bool(const aiNode&)> callback) const;
		void PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<Bool(const aiNode&)> callback) const;

		static StandardUniform MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property);
		static StandardAttribute MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property);
		static StandardUniform MapTextureTypeToUniform(TextureType textureType);
		static StandardAttribute MapTextureTypeToAttribute(TextureType textureType);
		static UV2Array* GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property);
		static const std::string* GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property);
		static const std::string* GetBuiltinVariableNameForTextureType(TextureType textureType);
		static TextureType ConvertAITextureKeyToTextureType(Int32 aiTextureKey);
		static Int32 ConvertTextureTypeToAITextureKey(TextureType textureType);

		static Bool HasOddReflections(Matrix4x4& mat);

	public:

		SceneObjectSharedPtr LoadModelDirect(const std::string& modelPath, Real importScale, Bool castShadows, Bool receiveShadows, Bool preserveFBXPivots);
		AnimationSharedPtr LoadAnimation(const std::string& filePath, Bool addLoopPadding, Bool preserveFBXPivots);

	};
}

#endif
