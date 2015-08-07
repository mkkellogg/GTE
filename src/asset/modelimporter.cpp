#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <unordered_map>

#include "graphics/stdattributes.h"
#include <IL/il.h>
#include "assimp/cimport.h"
#include "assimp/config.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#include "modelimporter.h"
#include "importutil.h"
#include "engine.h"
#include "object/engineobjectmanager.h"
#include "object/shaderorganizer.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/bone.h"
#include "graphics/animation/vertexbonemap.h"
#include "graphics/animation/sceneobjectskeletonnode.h"
#include "graphics/animation/animation.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/uv/uv2array.h"
#include "graphics/render/material.h"
#include "graphics/image/rawimage.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
#include "filesys/filesystem.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "geometry/matrix4x4.h"
#include "base/longmask.h"
#include "util/time.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "error/errormanager.h"
#include "util/engineutility.h"

namespace GTE
{

/**
 * Default constructor.
 */
ModelImporter::ModelImporter()
{
	importer = nullptr;
}

/**
 * Clean-up.
 */
ModelImporter::~ModelImporter()
{
	SAFE_DELETE(importer);
}

/**
 * Initialize the Assimp importer object (if it has not already been done so).
 */
void ModelImporter::InitImporter()
{
	if(importer == nullptr)
	{
		importer = new(std::nothrow) Assimp::Importer();
	}

	ASSERT(importer != nullptr, "ModelImporter::InitImporter -> importer is null.");
}

/**
 * Load an Assimp compatible model/scene located at [filePath]. [filePath] Must be a native file-system
 * compatible path, so the the engine's FileSystem singleton should be used to derive the correct platform-specific
 * path before calling this method.
 */
const aiScene * ModelImporter::LoadAIScene(const std::string& filePath, Bool preserveFBXPivots)
{
	// the global Assimp scene object
	const aiScene* scene = nullptr;

	// Create an instance of the Assimp Importer class
	InitImporter();

	// Check if model file exists
	std::ifstream fin(filePath.c_str());
	if(!fin.fail())
	{
		fin.close();
	}
	else
	{
		std::string msg = std::string("ModelImporter -> Could not find file: ") + filePath;
		Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::ModelFileNotFound, msg);
		return nullptr;
	}

	// tell Assimp not to create extra nodes when importing FBX files
	importer->SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, preserveFBXPivots ? 1 : 0);

	// read the model file in from disk
	scene = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality );

	// If the import failed, report it
	if(!scene)
	{
		std::string msg = std::string("ModelImporter::LoadAIScene -> Could not import file: ") + std::string(importer->GetErrorString());
		Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::ModelFileLoadFailed, msg);
		return nullptr;
	}

	return scene;
}

/**
 * Load an Assimp compatible model/scene located at [modelPath]. [modelPath] Must be a native file-system
 * compatible path, so the the engine's FileSystem singleton should be used to derive the correct platform-specific
 * path before calling this method.
 *
 * [importScale] - Allows for the adjustment of the model's scale
 * [castShadows] - Show the model's meshes cast shadows after being loaded into the scene?
 * [receiveShadows] - Show the model's meshes receive shadows after being loaded into the scene?
 */
SceneObjectRef ModelImporter::LoadModelDirect(const std::string& modelPath, Real importScale, Bool castShadows, Bool receiveShadows, Bool preserveFBXPivots)
{
	FileSystem * fileSystem = FileSystem::Instance();
	std::string fixedModelPath = fileSystem->FixupPathForLocalFilesystem(modelPath);

	// the global Assimp scene object
	const aiScene* scene = LoadAIScene(fixedModelPath, preserveFBXPivots);

	if(scene != nullptr)
	{
		// the model has been loaded from disk into Assimp data structures, now convert to engine-native structures
		SceneObjectRef result = ProcessModelScene(fixedModelPath, *scene, importScale, castShadows, receiveShadows);
		return result;
	}
	else
	{
		return SceneObjectRef::Null();
	}
}

/**
 * Convert an Assimp aiScene structure into an engine-native scene hierarchy.
 *
 * [modelPath] - Native file-system compatible path that points to the model/scene file in the file system. This is
 * 				 necessary to know because this is one of the locations that will be searched to find auxiliary resources
 * 				 such as textures.
 * [scene] - The Assimp aiScene structure that has been loaded from disk.
 * [importScale] - The factor by which the original size of the model should be adjusted.
 * [castShadows] - Show the model's meshes cast shadows after being loaded into the scene?
 * [receiveShadows] - Show the model's meshes receive shadows after being loaded into the scene?
 */
SceneObjectRef ModelImporter::ProcessModelScene(const std::string& modelPath, const aiScene& scene, Real importScale, Bool castShadows, Bool receiveShadows) const
{
	// get a pointer to the Engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// container for MaterialImportDescriptor instances that describe the engine-native
	// materials that get created during the call to ProcessMaterials()
	std::vector<MaterialImportDescriptor> materialImportDescriptors;

	// verify that we have a valid scene
	NONFATAL_ASSERT_RTRN(scene.mRootNode != nullptr,"ModelImporter::ProcessModelScene -> Assimp scene root is null.", SceneObjectRef::Null(), true);
	SceneObjectRef root = objectManager->CreateSceneObject();
	NONFATAL_ASSERT_RTRN(root.IsValid(),"ModelImporter::ProcessModelScene -> Could not create root object.", SceneObjectRef::Null(), true);

	FileSystem * fileSystem = FileSystem::Instance();
	std::string fixedModelPath = fileSystem->FixupPathForLocalFilesystem(modelPath);

	// process all the Assimp materials in [scene] and create equivalent engine native materials.
	// store those materials and their properties in MaterialImportDescriptor instances, which get
	// added to [materialImportDescriptors]
	Bool processMaterialsSuccess = ProcessMaterials(fixedModelPath, scene, materialImportDescriptors);
	if(!processMaterialsSuccess)
	{
		Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::ProcessMaterialsFailed, "ModelImporter::ProcessModelScene -> ProcessMaterials() returned an error.");
		return SceneObjectRef::Null();
	}

	// deactivate the root scene object so that it is not immediately
	// active or visible in the scene after it has been loaded
	root->SetActive(false);
	Matrix4x4 baseTransform;

	// pull the skeleton data from the scene/model (if it exists)
	SkeletonRef skeleton = LoadSkeleton(scene);

	// container for all the SceneObject instances that get created during this process
	std::vector<SceneObjectRef> createdSceneObjects;

	// recursively move down the Assimp scene hierarchy and process each node one by one.
	// all instances of SceneObject that are generated get stored in [createdSceneObjects].
	// any time meshes or mesh renderers are created, the information in [materialImportDescriptors]
	// will be used to link their materials and textures as appropriate.
	RecursiveProcessModelScene(scene, *(scene.mRootNode), importScale, root,  materialImportDescriptors, skeleton, createdSceneObjects, castShadows, receiveShadows);

	// loop through each instance of SceneObject that was created in the call to RecursiveProcessModelScene()
	// and for each instance that contains a SkinnedMesh3DRenderer instance, clone the Skeleton instance
	// created earlier in the call to LoadSkeleton() and assign the cloned skeleton to that renderer.
	for(UInt32 s = 0; s < createdSceneObjects.size(); s++)
	{
		// does the SceneObject instance have a SkinnedMesh3DRenderer ?
		SkinnedMesh3DRendererRef renderer = createdSceneObjects[s]->GetSkinnedMesh3DRenderer();
		if(renderer.IsValid())
		{
			// clone [skeleton]
			SkeletonRef skeletonClone = objectManager->CloneSkeleton(skeleton);
			if(!skeletonClone.IsValid())
			{
				Debug::PrintWarning("ModelImporter::ProcessModelScene -> Could not clone scene skeleton.");
				continue;
			}

			// assign the clones skeleton to [renderer]
			renderer->SetSkeleton(skeletonClone);

			Matrix4x4 mat;
			createdSceneObjects[s]->GetTransform().CopyMatrix(mat);

			// if the transformation matrix for this scene object has an inverted scale, we need to process the
			// vertex bone map in reverse order. we pass the [reverseVertexOrder] flag to SetupVertexBoneMapForRenderer()
			Bool reverseVertexOrder = HasOddReflections(mat);
			SetupVertexBoneMapForRenderer(scene, skeletonClone, renderer, reverseVertexOrder);
		}
	}
	return root;
}

/**
 * Recursively traverse the Assimp scene/model hierarchy and convert Assimp objects into
 * engine-native objects. This method will create instances of SceneObject for each Assimp node,
 * and instances of Mesh3D for Assimp meshes. Additionally it will create either Mesh3DRenderer or
 * SkinnedMesh3DRenderer instances for those meshes, depending on whether or no they have
 * skeletal data.
 *
 * [scene] - The Assimp aiScene structure that has been loaded from disk.
 * [node] - The current Assimp node, represented by an aiNode structure
 * [scale] - The factor by which the original size of the model should be adjusted.
 * [parent] - The parent SceneObject instance to which any child instances should be attached.
 * [materialImportDescriptors] - container for MaterialImportDescriptor instances that describe the
 * 								 engine-native materials to be linked with any meshes that are created.
 * [skeleton] - Instance of Skeleton for this scene/model (may be null)
 * [createdSceneObjects] - Container in which to store reference to SceneObject instances that get created.
 * [castShadows] - Show the model's meshes cast shadows after being loaded into the scene?
 * [receiveShadows] - Show the model's meshes receive shadows after being loaded into the scene?
 */
void ModelImporter::RecursiveProcessModelScene(const aiScene& scene,
											   const aiNode& node,
											   Real scale,
											   SceneObjectRef parent,
											   std::vector<MaterialImportDescriptor>& materialImportDescriptors,
											   SkeletonRef skeleton,
											   std::vector<SceneObjectRef>& createdSceneObjects,
											   Bool castShadows,
											   Bool receiveShadows) const
{
	Matrix4x4 mat;
	aiMatrix4x4 matBaseTransformation = node.mTransformation;
	ImportUtil::ConvertAssimpMatrix(matBaseTransformation,mat);

	// get a pointer to the Engine's object manager
	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();

	// create new scene object to hold the Mesh3D object and its renderer
	SceneObjectRef sceneObject = engineObjectManager->CreateSceneObject();
	NONFATAL_ASSERT(sceneObject.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Could not create scene object.", false);

	// determine if [skeleton] is valid
	Bool hasSkeleton = skeleton.IsValid() && skeleton->GetBoneCount() ? true : false;
	Bool requiresSkinnedRenderer = false;

	Mesh3DRenderer * rendererPtr = nullptr;
	SkinnedMesh3DRendererRef skinnedMeshRenderer;
	Mesh3DRendererRef meshRenderer;

	std::vector<UInt32> boneCounts;

	// are there any meshes in the model/scene?
	if(node.mNumMeshes > 0)
	{
		// loop through each mesh on this node and check for any bones.
		// if there is a mesh with bones, then we will create a SkinnedMesh3DRenderer
		// for all the meshes on this node.
		for (UInt32 n=0; n < node.mNumMeshes; n++)
		{
			UInt32 sceneMeshIndex = node.mMeshes[n];
			const aiMesh* mesh = scene.mMeshes[sceneMeshIndex];
			boneCounts.push_back(mesh->mNumBones);
			if(mesh->mNumBones > 0)requiresSkinnedRenderer = true && hasSkeleton;
		}

		// create a containing Mesh3D object that will hold all sub-meshes created for this node.
		// for each Assimp mesh, one SubMesh3D will be created added to the Mesh3D instance.
		Mesh3DRef mesh3D = engineObjectManager->CreateMesh3D(node.mNumMeshes);
		NONFATAL_ASSERT(mesh3D.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Could not create Mesh3D object.", false);

		// initialize the new Mesh3D instance
		Bool meshInitSuccess = mesh3D->Init();
		NONFATAL_ASSERT(meshInitSuccess,"ModelImporter::RecursiveProcessModelScene -> Unable to init Mesh3D object.", false);

		// if there are meshes with bones on this node, then we create a SkinnedMesh3DRenderer instead of a Mesh3DRenderer
		if(requiresSkinnedRenderer)
		{
			skinnedMeshRenderer = engineObjectManager->CreateSkinnedMesh3DRenderer();
			NONFATAL_ASSERT(skinnedMeshRenderer.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Could not create SkinnedMesh3DRenderer object.", false);
			rendererPtr = (Mesh3DRenderer*)skinnedMeshRenderer.GetPtr();

			// set the vertex bone map for each sub renderer to "none" (-1)
			// this means skinning for a given sub-mesh will be turned off until
			// a valid VertexBoneMap instance is set for it
			for (UInt32 n=0; n < node.mNumMeshes; n++)
			{
				skinnedMeshRenderer->MapSubMeshToVertexBoneMap(n,-1);
			}
		}
		else
		{
			meshRenderer = engineObjectManager->CreateMesh3DRenderer();
			NONFATAL_ASSERT(meshRenderer.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Could not create Mesh3DRenderer object.", false);
			rendererPtr = meshRenderer.GetPtr();
		}

		// loop through each Assimp mesh attached to the current Assimp node and
		// create a SubMesh3D instance for it
		for (UInt32 n=0; n < node.mNumMeshes; n++)
		{
			// get the index of the sub-mesh in the master list of meshes
			UInt32 sceneMeshIndex = node.mMeshes[n];

			// get a pointer to the Assimp mesh
			const aiMesh* mesh = scene.mMeshes[sceneMeshIndex];
			NONFATAL_ASSERT(mesh != nullptr, "ModelImporter::RecursiveProcessModelScene -> Assimp node mesh is null.", true);

			Int32 materialIndex = mesh->mMaterialIndex;
			MaterialImportDescriptor& materialImportDescriptor = materialImportDescriptors[materialIndex];
			MaterialRef material = materialImportDescriptor.meshSpecificProperties[sceneMeshIndex].material;
			NONFATAL_ASSERT(material.IsValid(),"ModelImporter::RecursiveProcessModelScene -> nullptr Material object encountered.", true);

			// add the material to the mesh renderer
			rendererPtr->AddMaterial(material);

			// if the transformation matrix for this node has an inverted scale, we need to process the mesh
			// differently or else it won't display correctly. we pass the [invert] flag to ConvertAssimpMesh()
			Bool invert = HasOddReflections(mat);
			// convert Assimp mesh to a Mesh3D object
			SubMesh3DRef subMesh3D = ConvertAssimpMesh(sceneMeshIndex, scene, materialImportDescriptor, invert);
			NONFATAL_ASSERT(subMesh3D.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.", false);

			// add the mesh to the newly created scene object
			mesh3D->SetSubMesh(subMesh3D, n);
		}

		Mesh3DFilterRef filter = engineObjectManager->CreateMesh3DFilter();
		NONFATAL_ASSERT(filter.IsValid(),"ModelImporter::RecursiveProcessModelScene -> Unable to create mesh#D filter object.", false);

		// set shadow properties
		filter->SetCastShadows(castShadows);
		filter->SetReceiveShadows(receiveShadows);
		filter->SetMesh3D(mesh3D);
		sceneObject->SetMesh3DFilter(filter);

		// set the SkinnedMesh3DRenderer instance and Mesh3D instance if any meshes on
		// this node have bones
		if(requiresSkinnedRenderer)
		{
			// for each mesh that has bones, activate the vertex bone map for the corresponding sub-renderer
			for (UInt32 n=0; n < node.mNumMeshes; n++)
			{
				if(boneCounts[n] > 0)skinnedMeshRenderer->MapSubMeshToVertexBoneMap(n, node.mMeshes[n]);
			}

			sceneObject->SetSkinnedMesh3DRenderer(skinnedMeshRenderer);
		}
		// set the Mesh3DRenderer instance and Mesh3D instance
		else
		{
			sceneObject->SetMesh3DRenderer(meshRenderer);
		}
	}

	// update the scene object's local transform
	sceneObject->GetTransform().SetTo(mat);

	std::string nodeName(node.mName.C_Str());
	sceneObject->SetName(nodeName);
	parent->AddChild(sceneObject);
	createdSceneObjects.push_back(sceneObject);

	if(hasSkeleton)
	{
		Int32 nodeMapping = skeleton->GetNodeMapping(nodeName);
		if(nodeMapping>=0)
		{
			SkeletonNode * node = skeleton->GetNodeFromList(nodeMapping);
			if(node != nullptr)
			{
				node->InitialTransform = mat;

				Vector3 scale;
				Vector3 translation;
				Quaternion rotation;

				// set the initial transformation properties
				mat.Decompose(translation,rotation,scale);
				node->InitialTranslation = translation;
				node->InitialRotation = rotation;
				node->InitialScale = scale;

				// if this skeleton node has a SceneObject target, then set it to [sceneObject]
				SceneObjectSkeletonNode *soskNode = dynamic_cast<SceneObjectSkeletonNode*>(node);
				if(soskNode != nullptr)
				{
					soskNode->Target = sceneObject;
				}
			}
		}
	}

	for(UInt32 i=0; i <node.mNumChildren; i++)
	{
		const aiNode *childNode = node.mChildren[i];
		if(childNode != nullptr)RecursiveProcessModelScene(scene, *childNode, scale, sceneObject, materialImportDescriptors, skeleton, createdSceneObjects, castShadows, receiveShadows);
	}
}

/**
 * Convert an Assimp mesh to an engine-native SubMesh3D instance.
 *
 * [meshIndex] - The index of the target Assimp mesh in the scene's list of meshes
 * [scene] - The Assimp scene/model.
 * [materialImportDescriptor] - Descriptor for the mesh's material.
 * [invert] - If true it means the mesh has an inverted scale transformation to deal with
 */
SubMesh3DRef ModelImporter::ConvertAssimpMesh(UInt32 meshIndex, const aiScene& scene, MaterialImportDescriptor& materialImportDescriptor, Bool invert) const
{
	NONFATAL_ASSERT_RTRN(meshIndex < scene.mNumMeshes, "ModelImporter::ConvertAssimpMesh -> mesh index is out of range.", SubMesh3DRef::Null(), true);

	UInt32 vertexCount = 0;
	aiMesh & mesh = *scene.mMeshes[meshIndex];

	// get the vertex count for the mesh
	vertexCount = mesh.mNumFaces * 3;

	// create a set of standard attributes that will dictate the standard attributes
	// to be used by the Mesh3D object created by this function.
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();

	// all meshes must have vertex positions
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

	Int32 diffuseTextureUVIndex = -1;
	// update the StandardAttributeSet to contain appropriate attributes (UV coords) for a diffuse texture
	if(materialImportDescriptor.meshSpecificProperties[meshIndex].UVMappingHasKey(TextureType::Diffuse))
	{
		StandardAttributes::AddAttribute(&meshAttributes, MapTextureTypeToAttribute(TextureType::Diffuse));
		diffuseTextureUVIndex = materialImportDescriptor.meshSpecificProperties[meshIndex].uvMapping[TextureType::Diffuse];
	}

	// add normals & tangents regardless of whether the mesh has them or not. if the mesh does not
	// have them, they can be calculated
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Tangent);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);

	// if the Assimp mesh's material specifies vertex colors, add vertex colors
	// to the StandardAttributeSet
	if(materialImportDescriptor.meshSpecificProperties[meshIndex].vertexColorsIndex >= 0)
	{
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	}

	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();

	// create Mesh3D object with the constructed StandardAttributeSet
	SubMesh3DRef mesh3D = engineObjectManager->CreateSubMesh3D(meshAttributes);
	NONFATAL_ASSERT_RTRN(mesh3D.IsValid(),"ModelImporter::ConvertAssimpMesh -> Could not create Mesh3D object.", SubMesh3DRef::Null(), false);

	Bool initSuccess = mesh3D->Init(vertexCount);

	// make sure allocation of required number of vertex attributes is successful
	if(!initSuccess)
	{
		engineObjectManager->DestroySubMesh3D(mesh3D);
		Debug::PrintError("ModelImporter::ConvertAssimpMesh -> Could not init mesh.");
		return SubMesh3DRef::Null();
	}

	Int32 vertexIndex = 0;

	// loop through each face in the mesh and copy relevant vertex attributes
	// into the newly created Mesh3D object
	for (UInt32 faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = mesh.mFaces + faceIndex;

		Int32 start, end, inc;
		if(!invert)
		{
			start = face->mNumIndices-1;end = -1;inc = -1;
		}
		else
		{
			start = 0;end = face->mNumIndices;inc = 1;
		}

		// ** IMPORTANT ** Normally we iterate through face vertices in reverse order. This is
		// necessary because vertices are stored in counter-clockwise order for each face.
		// if [invert] == true, then we instead iterate in forward order
		for( Int32 i = start; i != end; i+=inc)
		{
			Int32 vIndex = face->mIndices[i];

			aiVector3D srcPosition = mesh.mVertices[vIndex];

			// copy vertex position
			mesh3D->GetPostions()->GetPoint(vertexIndex)->Set(srcPosition.x,srcPosition.y,srcPosition.z);

			// copy mesh normals
			if(mesh.mNormals != nullptr)
			{
				aiVector3D& srcNormal = mesh.mNormals[vIndex];
				Vector3 normalCopy(srcNormal.x, srcNormal.y, srcNormal.z);
				mesh3D->GetVertexNormals()->GetVector(vertexIndex)->Set(normalCopy.x,normalCopy.y,normalCopy.z);
			}

			// copy vertex colors (if present)
			Int32 c = materialImportDescriptor.meshSpecificProperties[meshIndex].vertexColorsIndex;
			if(c >=0)
			{
				mesh3D->GetColors()->GetColor(vertexIndex)->Set(mesh.mColors[c]->r,mesh.mColors[c]->g,mesh.mColors[c]->b,mesh.mColors[c]->a);
			}

			// copy relevant data for diffuse texture (UV coords)
			if(diffuseTextureUVIndex >= 0)
			{
				UV2Array *uvs = GetMeshUVArrayForShaderMaterialCharacteristic(*mesh3D,ShaderMaterialCharacteristic::DiffuseTextured);
				if(materialImportDescriptor.meshSpecificProperties[meshIndex].invertVCoords)uvs->GetCoordinate(vertexIndex)->Set(mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].x, 1-mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].y);
				else uvs->GetCoordinate(vertexIndex)->Set(mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].x, mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].y);
			}

			vertexIndex++;
		}
	}
	if(invert)mesh3D->SetInvertNormals(true);
	mesh3D->SetNormalsSmoothingThreshold(80);
	return mesh3D;
}

/**
 * Process the Assimp materials (instances of aiMaterial) in the Assimp scene [scene]. This method loops through each
 * Assimp material and then examines which Assimp meshes use it. For each Assimp mesh that uses an Assimp material,
 * a unique (and equivalent) engine-native Material instance is created.
 *
 * For each mesh that uses a given Assimp material, we MUST create a unique engine-native Material object. Engine-native
 * Material objects are linked to shaders, and since different meshes may have different attributes, they may potentially
 * require DIFFERENT SHADERS. For example: Two meshes may share an Assimp material, but only one mesh might have vertex
 * colors. In this case two different shaders are needed: one that supports vertex colors and one that doesn't require them.
 *
 * [modelPath] - Native file-system compatible path that points to the model file in the file system.
 * [scene] - The Assimp model/scene.
 * [materialImportDescriptors] - A vector of MaterialImportDescriptor structures that will be populated by ProcessMaterials().
 */
Bool ModelImporter::ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const
{
	// TODO: Implement support for embedded textures
	if (scene.HasTextures())
	{
		Debug::PrintError("ModelImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();
	FileSystem * fileSystem = FileSystem::Instance();
	std::string fixedModelPath = fileSystem->FixupPathForLocalFilesystem(modelPath);

	// loop through each scene material and extract relevant textures and
	// other properties and create a MaterialDescriptor object that will hold those
	// properties and all corresponding Material objects
	for (UInt32 m=0; m < scene.mNumMaterials; m++)
	{
		aiString aiTexturePath;

		aiMaterial * assimpMaterial = scene.mMaterials[m];
		NONFATAL_ASSERT_RTRN(assimpMaterial != nullptr, "ModelImporter::ProcessMaterials -> Scene contains a null material.", false, true);

		aiString mtName;
		assimpMaterial->Get(AI_MATKEY_NAME,mtName);

		// build an import descriptor for this material
		MaterialImportDescriptor materialImportDescriptor;
		GetImportDetails(assimpMaterial, materialImportDescriptor, scene);

		aiReturn texFound = AI_SUCCESS;

		TextureRef diffuseTexture;
		//	TextureRef bumpTexture;
		//	TextureRef specularTexture;

		// get diffuse texture (for now support only 1)
		texFound = assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		if (texFound == AI_SUCCESS)diffuseTexture = LoadAITexture(*assimpMaterial, aiTextureType_DIFFUSE, fixedModelPath);
		if(!diffuseTexture.IsValid())
		{
			std::string msg = "ModelImporter::ProcessMaterials -> Could not load diffuse texture: ";
			msg += aiTexturePath.C_Str();
			Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::MaterialImportFailure, msg);
			materialImportDescriptors.clear();
			return false;
		}

		// loop through each mesh in the scene and check if it uses [material]. If so,
		// create a unique Material object for the mesh and attach it to [materialImportDescriptor]
		//
		// The [materialImportDescriptor] structure describes the unique per-mesh properties we need to be
		// concerned about when creating unique instances of a material for a mesh.
		for(UInt32 i = 0; i < scene.mNumMeshes; i++)
		{
			if(materialImportDescriptor.UsedByMesh(i))
			{
				// see if we can match a loaded shader to the properties of this material and the current mesh
				ShaderRef loadedShader = engineObjectManager->GetLoadedShader(materialImportDescriptor.meshSpecificProperties[i].shaderProperties);

				// if we can't find a loaded shader that matches the properties of this material and
				// the current mesh...well we can't really load this material
				if(!loadedShader.IsValid())
				{
					std::string msg = "Could not find loaded shader for: ";
					msg += std::bitset<64>(materialImportDescriptor.meshSpecificProperties[i].shaderProperties).to_string();
					Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::MaterialShaderMatchFailure, msg);
					materialImportDescriptors.clear();
					return false;
				}

				// create a new Material engine object
				MaterialRef newMaterial = engineObjectManager->CreateMaterial(mtName.C_Str(),loadedShader);
				if(!newMaterial.IsValid())
				{
					std::string msg = "ModelImporter::ProcessMaterials -> Could not create new Material object.";
					Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::MaterialImportFailure, msg);
					materialImportDescriptors.clear();
					return false;
				}

				// map new material to its corresponding mesh
				materialImportDescriptor.meshSpecificProperties[i].material = newMaterial;

				// if there is a diffuse texture, set it up in the new material
				if(diffuseTexture.IsValid())
				{
					// Add [diffuseTexture] to the new material (and for the appropriate shader variable), and store
					// Assimp UV channel for it in [materialImportDescriptor] for later processing of the mesh
					Bool setupSuccess = SetupMeshSpecificMaterialWithTexture(*assimpMaterial, TextureType::Diffuse, diffuseTexture, i, materialImportDescriptor);
					if(!setupSuccess)
					{
						std::string msg ="ModelImporter::ProcessMaterials -> Could not set up diffuse texture.";
						Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::MaterialImportFailure, msg);
						materialImportDescriptors.clear();
						return false;
					}
				}
			}
		}

		// add the new MaterialImportDescriptor instance to [materialImportDescriptors]
		materialImportDescriptors.push_back(materialImportDescriptor);
	}

	return true;
}

/**
 * Take an Assimp material [assimpMaterial] and use its properties to create and load an engine-native Texture instance
 * for it that matches the type specified by [textureType].
 *
 * This method looks in two places in the file system for the image files for the texture:
 *
 *    1. Using the full path that is specified in the [assimpMaterial] structure.
 *    2. In [modelPath], which is the location in the file system of model/scene to which [assimpMaterial] belongs.
 *
 * [modelPath] - Native file-system compatible path that points to the model file in the file system.
 * [assimpMaterial] - The Assimp material.
 * [textureType] - The type of texture to look for (diffuse, specular, normal map, etc...)
 */
TextureRef ModelImporter::LoadAITexture(aiMaterial& assimpMaterial, aiTextureType textureType, const std::string& modelPath) const
{
	// temp variables
	TextureRef texture;
	aiString aiTexturePath;
	aiReturn texFound = AI_SUCCESS;

	// get a pointer to the engine's object manager
	EngineObjectManager * engineObjectManager = Engine::Instance()->GetEngineObjectManager();
	FileSystem * fileSystem = FileSystem::Instance();

	// get the path to the directory that contains the scene/model
	std::string fixedModelPath = fileSystem->FixupPathForLocalFilesystem(modelPath);
	std::string modelDirectory = fileSystem->GetBasePath(fixedModelPath);
	
	// retrieve the first texture descriptor (at index 0) matching [textureType] from the Assimp material
	texFound = assimpMaterial.GetTexture(textureType, 0, &aiTexturePath);

	NONFATAL_ASSERT_RTRN(texFound == AI_SUCCESS, "ModelImporter::LoadAITexture -> Assimp material does not have desired texture type.", TextureRef::Null(), ModelImporterErrorCodes::AssimpTextureNotFound);

	// build the full path to the texture image as specified by the Assimp material
	std::string texPath = fileSystem->FixupPathForLocalFilesystem(std::string(aiTexturePath.data));
	std::string fullTextureFilePath = fileSystem->ConcatenatePaths(modelDirectory, texPath);

	TextureAttributes texAttributes;
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;

	// check if the file specified by the full path in the Assimp material exists
	if(fileSystem->FileExists(fullTextureFilePath))
	{
		texture = engineObjectManager->CreateTexture(fullTextureFilePath.c_str(),texAttributes);
	}
	// if it does not exist, try looking for the texture image file in the model's directory
	else
	{
		// get just the filename portion of the path
		std::string filename = fileSystem->GetFileName(fullTextureFilePath);
		if(!(filename.length() <= 0))
		{
			// concatenate the file name with the model's directory location
			filename = fileSystem->ConcatenatePaths(modelDirectory, filename);

			// check if the image file is in the same directory as the model and if so, load it
			if(fileSystem->FileExists(filename))
			{
				texture = engineObjectManager->CreateTexture(filename.c_str(),texAttributes);
			}
		}
	}

	// did texture fail to load?
	if(!texture.IsValid())
	{
		std::string msg = std::string("ModelImporter::LoadAITexture -> Could not load texture file: ") + fullTextureFilePath;
		Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::TextureFileLoadFailed, msg);
		return TextureRef::Null();
	}

	return texture;
}

/**
 * Set the material for the mesh specified by [meshIndex] in a material import descriptor [materialImportDesc] with an instance of
 * Texture that has already been loaded [texture]. This method determines the correct shader variable name for the texture based on
 * the type of texture [textureType], and sets that variable in the material for the mesh specified by [meshIndex] with [texture].
 * The method then locates the Assimp UV data for that texture and stores that in the mesh-specific properties of
 * [materialImportDesc].
 */
Bool ModelImporter::SetupMeshSpecificMaterialWithTexture(const aiMaterial& assimpMaterial, TextureType textureType, const TextureRef texture,
		 	 	 	 	 	 	 	 	 	 	 	 	 UInt32 meshIndex, MaterialImportDescriptor& materialImportDesc) const
{
	// get the Assimp material key for textures of type [textureType]
	UInt32 aiTextureKey = ConvertTextureTypeToAITextureKey(textureType);

	// get the name of the shader uniform that handles textures of [textureType]
	std::string textureName = GetBuiltinVariableNameForTextureType(textureType);

	// if we can't find a shader variable for the diffuse texture, then the load of this material has failed
	NONFATAL_ASSERT_RTRN(!textureName.empty(), "ModelImporter::SetupImportedMaterialTexture -> Could not locate shader variable for texture.", false, true);

	// set the diffuse texture in the material for the mesh specified by [meshIndex]
	materialImportDesc.meshSpecificProperties[meshIndex].material->SetTexture(texture, textureName);

	Int32 mappedIndex;

	// get the Assimp UV channel for the texture. the mapping will be used later when
	// processing the meshes in the scene
	if(AI_SUCCESS==aiGetMaterialInteger(&assimpMaterial,AI_MATKEY_UVWSRC(aiTextureKey,0),&mappedIndex))
		materialImportDesc.meshSpecificProperties[meshIndex].uvMapping[textureType] = mappedIndex;
	else
		materialImportDesc.meshSpecificProperties[meshIndex].uvMapping[textureType] = 0;

	return true;
}

/**
 * Get the global import properties for an Assimp material [mtl], and store in the global section  of the
 * supplied MaterialImportDescriptor instance [materialImportDesc].
 *
 * Additionally, get the mesh-specific properties for the Assimp material and store in the mesh-specific
 * properties section of the supplied MaterialImportDescriptor instance.
 */
void ModelImporter::GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene)
{
	LongMask flags = LongMaskUtil::CreateLongMask();
	aiString path;
	aiColor4t<Real> color;

	// automatically give normals to all materials & meshes (if a mesh doesn't have them by
	// default, they will be calculated)
	LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialCharacteristic::VertexNormals);

	// check for a diffuse texture
	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialCharacteristic::DiffuseTextured);
	}

	/*if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialProperty::SpecularTextured);
	}

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialProperty::Bumped);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialProperty::DiffuseColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialProperty::SpecularColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
	{
		LongMaskUtil::SetBit(&flags, (Int16)ShaderMaterialProperty::EmissiveColored);
	}*/

	// Even though multiple meshes may share an Assimp material, that doesn't necessarily
	// mean they can share a Material object. A Material object is linked to a shader
	// so if, for example, multiple Assimp meshes share a single Assimp material, but
	// only one of those meshes supplies vertex colors, then we actually need two Material objects
	// since the shaders will be different.
	//
	// This loop runs through each Assimp mesh that uses [mtl] and determines the unique
	// material properties of that mesh to form a final LongMask value that holds the
	// active ShaderMaterialCharacteristic values for that mesh.
	for(UInt32 i = 0; i < scene.mNumMeshes; i++)
	{
		// copy the existing set of ShaderMaterialCharacteristic values
		LongMask meshFlags = flags;
		// get mesh
		const aiMesh * mesh = scene.mMeshes[i];

		// does the current mesh use [mtl] ?
		if(scene.mMaterials[mesh->mMaterialIndex] == mtl)
		{
			// for now only support one set of vertex colors, and look at index 0 for it
			if(mesh->HasVertexColors(0))
			{
				LongMaskUtil::SetBit(&meshFlags, (Int16)ShaderMaterialCharacteristic::VertexColors);
				materialImportDesc.meshSpecificProperties[i].vertexColorsIndex = 0;
			}
			// set mesh specific ShaderMaterialCharacteristic values
			materialImportDesc.meshSpecificProperties[i].shaderProperties = meshFlags;
		}
	}
}

void ModelImporter::SetupVertexBoneMapForRenderer(const aiScene& scene, SkeletonRef skeleton, SkinnedMesh3DRendererRef target, Bool reverseVertexOrder) const
{
	for(UInt32 m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != nullptr && cMesh->mNumBones > 0)
		{
			VertexBoneMap indexBoneMap(cMesh->mNumVertices, cMesh->mNumVertices);

			Bool mapInitSuccess = indexBoneMap.Init();
			if(!mapInitSuccess)
			{
				Debug::PrintError("ModelImporter::SetupVertexBoneMapForRenderer -> Could not initialize index bone map.");
			}

			SetupVertexBoneMapMappingsFromAIMesh(skeleton, *cMesh, indexBoneMap);

			VertexBoneMap * fullBoneMap = ExpandIndexBoneMapping( indexBoneMap, *cMesh, reverseVertexOrder);
			if(fullBoneMap == nullptr)
			{
				Debug::PrintError("ModelImporter::SetupVertexBoneMapForRenderer -> Could not create full vertex bone map.");
			}

			target->AddVertexBoneMap(fullBoneMap);
		}
		else
		{
			target->AddVertexBoneMap(nullptr);
		}
	}
}

SkeletonRef ModelImporter::LoadSkeleton(const aiScene& scene) const
{
	UInt32 boneCount = CountBones(scene);
	if(boneCount <=0 )
	{
		return SkeletonRef::Null();
	}

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	SkeletonRef target = objectManager->CreateSkeleton(boneCount);
	NONFATAL_ASSERT_RTRN(target.IsValid(),"ModelImporter::LoadSkeleton -> Could not allocate skeleton.",SkeletonRef::Null(), false);

	Bool skeletonInitSuccess = target->Init();
	if(!skeletonInitSuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Unable to initialize skeleton.");
		objectManager->DestroySkeleton(target);
		return SkeletonRef::Null();
	}

	UInt32 boneIndex = 0;
	for(UInt32 m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != nullptr && cMesh->mNumBones > 0)
		{

			AddMeshBoneMappingsToSkeleton(target, *cMesh, boneIndex);
		}
	}

	Bool hierarchysuccess = CreateAndMapNodeHierarchy(target, scene);
	if(!hierarchysuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not create node hierarchy.");
		objectManager->DestroySkeleton(target);
		return SkeletonRef::Null();
	}

	return target;
}

VertexBoneMap * ModelImporter::ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh, Bool reverseVertexOrder) const
{
	VertexBoneMap * fullBoneMap = new(std::nothrow) VertexBoneMap(mesh.mNumFaces * 3, mesh.mNumVertices);
	if(fullBoneMap == nullptr)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate vertex bone map.");
		return nullptr;
	}

	Bool mapInitSuccess = fullBoneMap->Init();
	if(!mapInitSuccess)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not initialize vertex bone map.");
		return nullptr;
	}

	unsigned fullIndex=0;
	for(UInt32 f = 0; f < mesh.mNumFaces; f++)
	{
		aiFace& face = mesh.mFaces[f];

		Int32 start, end, inc;
		if(!reverseVertexOrder)
		{
			start = face.mNumIndices-1;end = -1;inc = -1;
		}
		else
		{
			start = 0;end = face.mNumIndices;inc = 1;
		}
		// ** IMPORTANT ** Iterate through face vertices in reverse order. This is necessary because
		// vertices are stored in counter-clockwise order for each face. if [reverseVertexOrder] == true,
		// then we iterate in normal forward order
		for( Int32 i = start; i != end; i+=inc)
		{
			UInt32 vertexIndex = face.mIndices[i];
			fullBoneMap->GetDescriptor(fullIndex)->SetTo(indexBoneMap.GetDescriptor(vertexIndex));
			fullIndex++;
		}
	}

	return fullBoneMap;
}

void ModelImporter::AddMeshBoneMappingsToSkeleton(SkeletonRef skeleton, const aiMesh& mesh, UInt32& currentBoneIndex) const
{
	NONFATAL_ASSERT(skeleton.IsValid(), "ModelImporter::AddBoneMappings -> skeleton is invalid.", true);

	for(UInt32 b = 0; b < mesh.mNumBones; b++)
	{
		aiBone * cBone = mesh.mBones[b];
		if(cBone != nullptr)
		{
			std::string boneName = std::string(cBone->mName.C_Str());

			if(skeleton->GetBoneMapping(boneName) == -1)
			{
				skeleton->MapBone(boneName, currentBoneIndex);

				Matrix4x4 offsetMatrix;
				ImportUtil::ConvertAssimpMatrix(cBone->mOffsetMatrix, offsetMatrix);

				skeleton->GetBone(currentBoneIndex)->Name = boneName;
				skeleton->GetBone(currentBoneIndex)->ID = currentBoneIndex;
				skeleton->GetBone(currentBoneIndex)->OffsetMatrix.SetTo(offsetMatrix);

				/*printf("offset: %f,%f,%f,%f,   %f,%f,%f,%f,   %f,%f,%f,%f,   %f,%f,%f,%f\n", offsetMatrix.A0, offsetMatrix.A1, offsetMatrix.A2, offsetMatrix.A3,
																							  offsetMatrix.B0, offsetMatrix.B1, offsetMatrix.B2, offsetMatrix.B3,
																							  offsetMatrix.C0, offsetMatrix.C1, offsetMatrix.C2, offsetMatrix.C3,
																							  offsetMatrix.D0, offsetMatrix.D1, offsetMatrix.D2, offsetMatrix.D3);*/


				currentBoneIndex++;
			}
		}
	}
}

void ModelImporter::SetupVertexBoneMapMappingsFromAIMesh(SkeletonRefConst skeleton, const aiMesh& mesh, VertexBoneMap& vertexIndexBoneMap) const
{
	NONFATAL_ASSERT(skeleton.IsValid(), "ModelImporter::AddBoneMappings -> skeleton is invalid.", true);

	for(UInt32 b = 0; b < mesh.mNumBones; b++)
	{
		aiBone * cBone = mesh.mBones[b];
		if(cBone != nullptr)
		{
			std::string boneName = std::string(cBone->mName.C_Str());
			UInt32 boneIndex = skeleton->GetBoneMapping(boneName);

			for(UInt32 w = 0; w < cBone->mNumWeights; w++)
			{
				aiVertexWeight& weightDesc = cBone->mWeights[w];

				UInt32 vertexID = weightDesc.mVertexId;
				Real weight = weightDesc.mWeight;

				VertexBoneMap::VertexMappingDescriptor * desc = vertexIndexBoneMap.GetDescriptor(vertexID);
				if(desc != nullptr && desc->BoneCount < Constants::MaxBonesPerVertex)
				{
					desc->UniqueVertexIndex = vertexID;
					desc->BoneIndex[desc->BoneCount] = boneIndex;
					desc->Weight[desc->BoneCount] = weight;
					desc->Name[desc->BoneCount] = boneName;
					desc->BoneCount++;
				}
			}
		}
	}
}

unsigned ModelImporter::CountBones(const aiScene& scene) const
{
	UInt32 boneCount = 0;
	std::unordered_map<std::string, UInt32> boneCountMap;
	for(UInt32 m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != nullptr && cMesh->mNumBones > 0)
		{
			for(UInt32 b = 0; b < cMesh->mNumBones; b++)
			{
				std::string boneName(cMesh->mBones[b]->mName.C_Str());
				if(boneCountMap.find(boneName) == boneCountMap.end())
				{
					boneCountMap[boneName] = 1;
					boneCount++;
				}
			}
		}
	}

	return boneCount;
}

Bool ModelImporter::CreateAndMapNodeHierarchy(SkeletonRef skeleton, const aiScene& scene) const
{
	SceneObjectSkeletonNode * skeletonNode = new(std::nothrow) SceneObjectSkeletonNode(SceneObjectRef::Null(), -1, "");
	if(skeletonNode == nullptr)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate skeleton root node.");
		return false;
	}

	Tree<SkeletonNode*>::TreeNode * lastNode = skeleton->CreateRoot(skeletonNode);
	if(lastNode == nullptr)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not create skeleton root node.");
		return false;
	}

	Skeleton * skeletonPtr = skeleton.GetPtr();
	Bool success = true;
	TraverseScene(scene, SceneTraverseOrder::PreOrder, [skeletonPtr, lastNode, &success](const aiNode& node) -> Bool
	{
		std::string boneName(node.mName.C_Str());
		Int32 mappedBoneIndex = skeletonPtr->GetBoneMapping(boneName);

		SceneObjectSkeletonNode * childSkeletonNode = new(std::nothrow) SceneObjectSkeletonNode(SceneObjectRef::Null(), mappedBoneIndex, boneName);
		if(childSkeletonNode == nullptr)
		{
			Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate skeleton child node.");
			success  = false;
			return false;
		}

		skeletonPtr->MapNode(boneName, skeletonPtr->GetNodeCount());
		skeletonPtr->AddNodeToList(childSkeletonNode);

		Tree<SkeletonNode*>::TreeNode * childNode = skeletonPtr->AddChild(lastNode, childSkeletonNode);
		if(childNode == nullptr)
		{
			Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not create skeleton child node.");
			success  = false;
			return false;
		}

		if(mappedBoneIndex >= 0)
		{
			Bone * bone = skeletonPtr->GetBone(mappedBoneIndex);
			bone->Node = childSkeletonNode;
		}

		return true;
	});

	return success;
}

AnimationRef ModelImporter::LoadAnimation (aiAnimation& animation, Bool addLoopPadding) const
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	NONFATAL_ASSERT_RTRN(objectManager != nullptr,"ModelImporter::LoadAnimation -> EngineObjectManager instance is null.", AnimationRef::Null(), true);

	Real ticksPerSecond = (Real)animation.mTicksPerSecond;

	// adding little extra time to the animation allows for the interpolation between the last
	// and the first frame, which smoothes out looping animations
	// TODO: figure out a better way to do this, possibly a setting for smoothing looped animations
	Real loopPadding = ticksPerSecond * .05f;
	Real durationTicks = (Real)animation.mDuration;

	if(addLoopPadding) durationTicks += loopPadding;

	NONFATAL_ASSERT_RTRN(ticksPerSecond > 0, "ModelImporter::LoadAnimation -> Ticks per second is 0.", AnimationRef::Null(), true);
	//Real duration = durationTicks / ticksPerSecond;

	AnimationRef animationRef = objectManager->CreateAnimation(durationTicks, ticksPerSecond);
	NONFATAL_ASSERT_RTRN(animationRef.IsValid(),"ModelImporter::LoadAnimation -> Unable to create Animation.", AnimationRef::Null(), false);

	Bool initSuccess = animationRef->Init(animation.mNumChannels);
	if(!initSuccess)
	{
		objectManager->DestroyAnimation(animationRef);
		Debug::PrintError("ModelImporter::LoadAnimation -> Unable to initialize Animation.");
		return AnimationRef::Null();
	}

	for(UInt32 n = 0; n < animation.mNumChannels; n++)
	{
		aiNodeAnim * nodeAnim = animation.mChannels[n];
		std::string nodeName(nodeAnim->mNodeName.C_Str());

		animationRef->SetChannelName(n,nodeName);

		//int nodeIndex = skeleton->GetNodeMapping(nodeName);
		Int32 nodeIndex = n;
		if(nodeIndex >= 0)
		{
			KeyFrameSet * keyFrameSet = animationRef->GetKeyFrameSet(nodeIndex);
			if(keyFrameSet == nullptr)
			{
				objectManager->DestroyAnimation(animationRef);
				std::string msg = std::string("ModelImporter::LoadAnimation -> nullptr KeyFrameSet encountered for: ") + nodeName;
				Debug::PrintError(msg);
				return AnimationRef::Null();
			}

			keyFrameSet->Used = true;

			for(UInt32 t = 0; t < nodeAnim->mNumPositionKeys; t++)
			{
				aiVectorKey& vectorKey = *(nodeAnim->mPositionKeys + t);

				TranslationKeyFrame keyFrame;
				keyFrame.NormalizedTime = (Real)vectorKey.mTime / durationTicks;
				keyFrame.RealTime = (Real)vectorKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (Real)vectorKey.mTime;
				keyFrame.Translation.Set(vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z);
				keyFrameSet->TranslationKeyFrames.push_back(keyFrame);
			}

			for(UInt32 s = 0; s < nodeAnim->mNumScalingKeys; s++)
			{

				aiVectorKey& vectorKey = *(nodeAnim->mScalingKeys + s);

				ScaleKeyFrame keyFrame;
				keyFrame.NormalizedTime = (Real)vectorKey.mTime / durationTicks;
				keyFrame.RealTime = (Real)vectorKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (Real)vectorKey.mTime;
				keyFrame.Scale.Set(vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z);
				keyFrameSet->ScaleKeyFrames.push_back(keyFrame);
			}

			for(UInt32 r = 0; r < nodeAnim->mNumRotationKeys; r++)
			{
				aiQuatKey& quatKey = *(nodeAnim->mRotationKeys + r);

				RotationKeyFrame keyFrame;
				keyFrame.NormalizedTime = (Real)quatKey.mTime / durationTicks;
				keyFrame.RealTime = (Real)quatKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (Real)quatKey.mTime;
				keyFrame.Rotation.Set(quatKey.mValue.x,quatKey.mValue.y,quatKey.mValue.z,quatKey.mValue.w );
				keyFrameSet->RotationKeyFrames.push_back(keyFrame);
			}
		}
	}

	return animationRef;
}

/*
 * Currently this loads only the first animation found in the model file.
 *
 */
AnimationRef ModelImporter::LoadAnimation(const std::string& filePath, Bool addLoopPadding, Bool preserveFBXPivots)
{
	InitImporter();

	const aiScene * scene = LoadAIScene(filePath, preserveFBXPivots);
	NONFATAL_ASSERT_RTRN(scene != nullptr, "ModelImporter::LoadAnimation -> Unable to load scene.", AnimationRef::Null(), false);

	NONFATAL_ASSERT_RTRN(scene->mNumAnimations > 0, "ModelImporter::LoadAnimation -> Model does not contain any animations.", AnimationRef::Null(), true);

	// only load the first animation
	AnimationRef animation = LoadAnimation(*(scene->mAnimations[0]), addLoopPadding);
	NONFATAL_ASSERT_RTRN(animation.IsValid(),"ModelImporter::LoadAnimation -> Unable to load Animation.", AnimationRef::Null(), false);

	return animation;
}

void ModelImporter::TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<Bool(const aiNode&)> callback) const
{
	if(scene.mRootNode != nullptr)
	{
		const aiNode& sceneRef = (const aiNode&)(*(scene.mRootNode));
		if(traverseOrder == SceneTraverseOrder::PreOrder)
			PreOrderTraverseScene(scene, sceneRef, callback);
	}
}

void ModelImporter::PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<Bool(const aiNode&)> callback) const
{
	Bool doContinue = callback(node);
	if(!doContinue)return;

	for(UInt32 i = 0; i < node.mNumChildren; i++)
	{
		aiNode* childNode = node.mChildren[i];
		if(childNode != nullptr)
		{
			PreOrderTraverseScene(scene, (const aiNode&)(*(childNode)), callback);
		}
	}
}

UV2Array* ModelImporter::GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property)
{
	switch(property)
	{
		case ShaderMaterialCharacteristic::DiffuseTextured:
			return mesh.GetUVs0();
		break;
		default:
			return nullptr;
		break;
	}

	return nullptr;
}

StandardUniform ModelImporter::MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property)
{
	switch(property)
	{
		case ShaderMaterialCharacteristic::DiffuseTextured:
			return StandardUniform::Texture0;
		break;
		default:
			return StandardUniform::_None;
		break;
	}

	return StandardUniform::_None;
}

StandardAttribute ModelImporter::MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property)
{
	switch(property)
	{
		case ShaderMaterialCharacteristic::DiffuseTextured:
			return StandardAttribute::UVTexture0;
		break;
		default:
			return StandardAttribute::_None;
		break;
	}

	return StandardAttribute::_None;
}

StandardUniform ModelImporter::MapTextureTypeToUniform(TextureType textureType)
{
	switch(textureType)
	{
		case TextureType::Diffuse:
			return StandardUniform::Texture0;
		break;
		default:
			return StandardUniform::_None;
		break;
	}

	return StandardUniform::_None;
}

StandardAttribute ModelImporter::MapTextureTypeToAttribute(TextureType textureType)
{
	switch(textureType)
	{
		case TextureType::Diffuse:
			return StandardAttribute::UVTexture0;
		break;
		default:
			return StandardAttribute::_None;
		break;
	}

	return StandardAttribute::_None;
}

std::string ModelImporter::GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property)
{

	StandardUniform uniform = MapShaderMaterialCharacteristicToUniform(property);

	if(uniform != StandardUniform::_None)
	{
		return StandardUniforms::GetUniformName(uniform);
	}

	return "";
}

std::string ModelImporter::GetBuiltinVariableNameForTextureType(TextureType textureType)
{

	StandardUniform uniform = MapTextureTypeToUniform(textureType);

	if(uniform != StandardUniform::_None)
	{
		return StandardUniforms::GetUniformName(uniform);
	}

	return "";
}

ModelImporter::TextureType ModelImporter::ConvertAITextureKeyToTextureType(Int32 aiTextureKey)
{
	TextureType textureType = TextureType::_None;
	if(aiTextureKey == aiTextureType_SPECULAR)textureType = TextureType::Specular;
	else if(aiTextureKey == aiTextureType_NORMALS)textureType = TextureType::BumpMap;
	else if(aiTextureKey == aiTextureType_DIFFUSE)textureType = TextureType::Diffuse;
	return textureType;

}

int ModelImporter::ConvertTextureTypeToAITextureKey(TextureType textureType)
{
	Int32 aiTextureKey = -1;
	if(textureType == TextureType::Specular)aiTextureKey = aiTextureType_SPECULAR;
	else if(textureType == TextureType::BumpMap)aiTextureKey = aiTextureType_NORMALS;
	else if(textureType == TextureType::Diffuse)aiTextureKey = aiTextureType_DIFFUSE;
	return aiTextureKey;
}

/*
 * Determine if [mat] has an odd number of reflections.
 */
Bool ModelImporter::HasOddReflections(Matrix4x4& mat)
{
	Real determinant = mat.CalculateDeterminant();
	if (determinant < 0.0)return true;
	return false;
}
}

