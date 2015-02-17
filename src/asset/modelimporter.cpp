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
#include "debug/gtedebug.h"
#include "error/errormanager.h"
#include "util/engineutility.h"

/**
 * Default constructor.
 */
ModelImporter::ModelImporter()
{
	importer = NULL;
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
bool ModelImporter::InitImporter()
{
	if(importer == NULL)
	{
		importer = new Assimp::Importer();
	}

	if(importer == NULL)return false;
	return true;
}

/**
 * Load an Assimp compatible model/scene located at [filePath]. [filePath] Must be a native file-system
 * compatible path, so the the engine's FileSystem singleton should be used to derive the correct platform-specific
 * path before calling this method.
 */
const aiScene * ModelImporter::LoadAIScene(const std::string& filePath, bool preserveFBXPivots)
{
	// the global Assimp scene object
	const aiScene* scene = NULL;

	// Create an instance of the Assimp Importer class
	bool initSuccess = InitImporter();
	ASSERT(initSuccess == true," ModelImporter::LoadAIScene -> Could not initialize Assimp importer." , NULL);

	// Check if model file exists
	std::ifstream fin(filePath.c_str());
	if(!fin.fail())
	{
		fin.close();
	}
	else
	{
		std::string msg = std::string("AssetImporter -> Could not find file: ") + filePath;
		Engine::Instance()->GetErrorManager()->SetAndReportWarning(ModelImporterErrorCodes::ModelFileNotFound, msg);
		return NULL;
	}

	// tell Assimp not to create extra nodes when importing FBX files
	importer->SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, preserveFBXPivots ? 1 : 0);

	// read the model file in from disk
	scene = importer->ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality );

	// If the import failed, report it
	if(!scene)
	{
		std::string msg = std::string("AssetImporter::LoadAIScene -> Could not import file: ") + std::string(importer->GetErrorString());
		Engine::Instance()->GetErrorManager()->SetAndReportWarning(ModelImporterErrorCodes::ModelFileLoadFailed, msg);
		return NULL;
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
SceneObjectRef ModelImporter::LoadModelDirect(const std::string& modelPath, float importScale, bool castShadows, bool receiveShadows, bool preserveFBXPivots)
{
	// the global Assimp scene object
	const aiScene* scene = LoadAIScene(modelPath, preserveFBXPivots);

	if(scene != NULL)
	{
		// the model has been loaded from disk into Assimp data structures, now convert to engine-native structures
		SceneObjectRef result =  ProcessModelScene(modelPath, *scene, importScale, castShadows, receiveShadows);
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
SceneObjectRef ModelImporter::ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale, bool castShadows, bool receiveShadows) const
{
	// get a pointer to the Engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// container for MaterialImportDescriptor instances that describe the engine-native
	// materials that get created during the call to ProcessMaterials()
	std::vector<MaterialImportDescriptor> materialImportDescriptors;

	// verify that we have a valid scene
	ASSERT(scene.mRootNode != NULL,"AssetImporter::ProcessModelScene -> Assimp scene root is NULL.", SceneObjectRef::Null());
	SceneObjectRef root = objectManager->CreateSceneObject();
	ASSERT(root.IsValid(),"AssetImporter::ProcessModelScene -> Could not create root object.", SceneObjectRef::Null());

	// process all the Assimp materials in [scene] and create equivalent engine native materials.
	// store those materials and their properties in MaterialImportDescriptor instances, which get
	// added to [materialImportDescriptors]
	bool processMaterialsSuccess = ProcessMaterials(modelPath, scene, materialImportDescriptors);
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
	for(unsigned int s = 0; s < createdSceneObjects.size(); s++)
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
			bool reverseVertexOrder = HasInvertedScale(mat);
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
											   float scale,
											   SceneObjectRef parent,
											   std::vector<MaterialImportDescriptor>& materialImportDescriptors,
											   SkeletonRef skeleton,
											   std::vector<SceneObjectRef>& createdSceneObjects,
											   bool castShadows,
											   bool receiveShadows) const
{
	Matrix4x4 mat;
	aiMatrix4x4 matBaseTransformation = node.mTransformation;
	ImportUtil::ConvertAssimpMatrix(matBaseTransformation,mat);

	// get a pointer to the Engine's object manager
	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();

	// create new scene object to hold the Mesh3D object and its renderer
	SceneObjectRef sceneObject = engineObjectManager->CreateSceneObject();
	ASSERT_RTRN(sceneObject.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Could not create scene object.");

	// determine if [skeleton] is valid
	bool hasSkeleton = skeleton.IsValid() && skeleton->GetBoneCount() ? true : false;
	bool requiresSkinnedRenderer = false;

	Mesh3DRenderer * rendererPtr = NULL;
	SkinnedMesh3DRendererRef skinnedMeshRenderer;
	Mesh3DRendererRef meshRenderer;

	std::vector<unsigned int> boneCounts;

	// are there any meshes in the model/scene?
	if(node.mNumMeshes > 0)
	{
		// loop through each mesh on this node and check for any bones.
		// if there is a mesh with bones, then we will create a SkinnedMesh3DRenderer
		// for all the meshes on this node.
		for (unsigned int n=0; n < node.mNumMeshes; n++)
		{
			unsigned int sceneMeshIndex = node.mMeshes[n];
			const aiMesh* mesh = scene.mMeshes[sceneMeshIndex];
			boneCounts.push_back(mesh->mNumBones);
			if(mesh->mNumBones > 0)requiresSkinnedRenderer = true && hasSkeleton;
		}

		// create a containing Mesh3D object that will hold all sub-meshes created for this node.
		// for each Assimp mesh, one SubMesh3D will be created added to the Mesh3D instance.
		Mesh3DRef mesh3D = engineObjectManager->CreateMesh3D(node.mNumMeshes);
		ASSERT_RTRN(mesh3D.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3D object.");

		// initialize the new Mesh3D instance
		bool meshInitSuccess = mesh3D->Init();
		ASSERT_RTRN(meshInitSuccess,"AssetImporter::RecursiveProcessModelScene -> Unable to init Mesh3D object.");

		// set shadow properties
		mesh3D->SetCastShadows(castShadows);
		mesh3D->SetReceiveShadows(receiveShadows);

		// if there are meshes with bones on this node, then we create a SkinnedMesh3DRenderer instead of a Mesh3DRenderer
		if(requiresSkinnedRenderer)
		{
			skinnedMeshRenderer = engineObjectManager->CreateSkinnedMesh3DRenderer();
			ASSERT_RTRN(skinnedMeshRenderer.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Could not create SkinnedMesh3DRenderer object.");
			rendererPtr = (Mesh3DRenderer*)skinnedMeshRenderer.GetPtr();

			// set the vertex bone map for each sub renderer to "none" (-1)
			// this means skinning for a given sub-mesh will be turned off until
			// a valid VertexBoneMap instance is set for it
			for (unsigned int n=0; n < node.mNumMeshes; n++)
			{
				skinnedMeshRenderer->MapSubMeshToVertexBoneMap(n,-1);
			}
		}
		else
		{
			meshRenderer = engineObjectManager->CreateMesh3DRenderer();
			ASSERT_RTRN(meshRenderer.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3DRenderer object.");
			rendererPtr = meshRenderer.GetPtr();
		}

		// loop through each Assimp mesh attached to the current Assimp node and
		// create a SubMesh3D instance for it
		for (unsigned int n=0; n < node.mNumMeshes; n++)
		{
			// get the index of the sub-mesh in the master list of meshes
			unsigned int sceneMeshIndex = node.mMeshes[n];

			// get a pointer to the Assimp mesh
			const aiMesh* mesh = scene.mMeshes[sceneMeshIndex];
			ASSERT_RTRN(mesh != NULL, "AssetImporter::RecursiveProcessModelScene -> Node mesh is NULL.");

			int materialIndex = mesh->mMaterialIndex;
			MaterialImportDescriptor& materialImportDescriptor = materialImportDescriptors[materialIndex];
			MaterialRef material = materialImportDescriptor.meshSpecificProperties[sceneMeshIndex].material;
			ASSERT_RTRN(material.IsValid(),"AssetImporter::RecursiveProcessModelScene -> NULL Material object encountered.");

			// add the material to the mesh renderer
			rendererPtr->AddMaterial(material);

			// if the transformation matrix for this node has an inverted scale, we need to process the mesh
			// differently or else it won't display correctly. we pass the [invert] flag to ConvertAssimpMesh()
			bool invert = HasInvertedScale(mat);
			// convert Assimp mesh to a Mesh3D object
			SubMesh3DRef subMesh3D = ConvertAssimpMesh(sceneMeshIndex, scene, materialImportDescriptor, invert);
			ASSERT_RTRN(subMesh3D.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.");

			// add the mesh to the newly created scene object
			mesh3D->SetSubMesh(subMesh3D, n);
		}

		Mesh3DFilterRef filter = engineObjectManager->CreateMesh3DFilter();
		ASSERT_RTRN(filter.IsValid(),"AssetImporter::RecursiveProcessModelScene -> Unable to create mesh#D filter object.");

		filter->SetMesh3D(mesh3D);
		sceneObject->SetMesh3DFilter(filter);

		// set the SkinnedMesh3DRenderer instance and Mesh3D instance if any meshes on
		// this node have bones
		if(requiresSkinnedRenderer)
		{
			// for each mesh that has bones, activate the vertex bone map for the corresponding sub-renderer
			for (unsigned int n=0; n < node.mNumMeshes; n++)
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
		int nodeMapping = skeleton->GetNodeMapping(nodeName);
		if(nodeMapping>=0)
		{
			SkeletonNode * node = skeleton->GetNodeFromList(nodeMapping);
			if(node != NULL)
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
				if(soskNode != NULL)
				{
					soskNode->Target = sceneObject;
				}
			}
		}
	}

	for(unsigned int i=0; i <node.mNumChildren; i++)
	{
		const aiNode *childNode = node.mChildren[i];
		if(childNode != NULL)RecursiveProcessModelScene(scene, *childNode, scale, sceneObject, materialImportDescriptors, skeleton, createdSceneObjects, castShadows, receiveShadows);
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
SubMesh3DRef ModelImporter::ConvertAssimpMesh(unsigned int meshIndex, const aiScene& scene, MaterialImportDescriptor& materialImportDescriptor, bool invert) const
{
	ASSERT(meshIndex < scene.mNumMeshes, "ModelImporter::ConvertAssimpMesh -> mesh index is out of range.", SubMesh3DRef::Null());

	unsigned int vertexCount = 0;
	aiMesh & mesh = *scene.mMeshes[meshIndex];

	// get the vertex count for the mesh
	vertexCount = mesh.mNumFaces * 3;

	// create a set of standard attributes that will dictate the standard attributes
	// to be used by the Mesh3D object created by this function.
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();

	// all meshes must have vertex positions
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

	int diffuseTextureUVIndex = -1;
	// update the StandardAttributeSet to contain appropriate attributes (UV coords) for a diffuse texture
	if(materialImportDescriptor.meshSpecificProperties[meshIndex].UVMappingHasKey(TextureType::Diffuse))
	{
		StandardAttributes::AddAttribute(&meshAttributes, MapTextureTypeToAttribute(TextureType::Diffuse));
		diffuseTextureUVIndex = materialImportDescriptor.meshSpecificProperties[meshIndex].uvMapping[TextureType::Diffuse];
	}

	// add normals regardless of whether the mesh has them or not. if the mesh does not
	// have them, they can be calculated
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	// if the Assimp mesh's material specifies vertex colors, add vertex colors
	// to the StandardAttributeSet
	if(materialImportDescriptor.meshSpecificProperties[meshIndex].vertexColorsIndex >= 0)
	{
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	}

	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();

	// create Mesh3D object with the constructed StandardAttributeSet
	SubMesh3DRef mesh3D = engineObjectManager->CreateSubMesh3D(meshAttributes);
	ASSERT(mesh3D.IsValid(),"AssetImporter::ConvertAssimpMesh -> Could not create Mesh3D object.", SubMesh3DRef::Null());

	bool initSuccess = mesh3D->Init(vertexCount);

	// make sure allocation of required number of vertex attributes is successful
	if(!initSuccess)
	{
		engineObjectManager->DestroySubMesh3D(mesh3D);
		Debug::PrintError("AssetImporter::ConvertAssimpMesh -> Could not init mesh.");
		return SubMesh3DRef::Null();
	}

	int vertexIndex = 0;

	// loop through each face in the mesh and copy relevant vertex attributes
	// into the newly created Mesh3D object
	for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = mesh.mFaces + faceIndex;

		int start, end, inc;
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
		for( int i = start; i != end; i+=inc)
		{
			int vIndex = face->mIndices[i];

			aiVector3D srcPosition = mesh.mVertices[vIndex];

			// copy vertex position
			mesh3D->GetPostions()->GetPoint(vertexIndex)->Set(srcPosition.x,srcPosition.y,srcPosition.z);

			// copy mesh normals
			if(mesh.mNormals != NULL)
			{
				aiVector3D& srcNormal = mesh.mNormals[vIndex];
				Vector3 normalCopy(srcNormal.x, srcNormal.y, srcNormal.z);
				mesh3D->GetVertexNormals()->GetVector(vertexIndex)->Set(normalCopy.x,normalCopy.y,normalCopy.z);
			}

			// copy vertex colors (if present)
			int c = materialImportDescriptor.meshSpecificProperties[meshIndex].vertexColorsIndex;
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
bool ModelImporter::ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors) const
{
	// TODO: Implement support for embedded textures
	if (scene.HasTextures())
	{
		Debug::PrintError("AssetImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	EngineObjectManager * engineObjectManager =  Engine::Instance()->GetEngineObjectManager();

	// loop through each scene material and extract relevant textures and
	// other properties and create a MaterialDescriptor object that will hold those
	// properties and all corresponding Material objects
	for (unsigned int m=0; m < scene.mNumMaterials; m++)
	{
		aiString aiTexturePath;

		aiMaterial * assimpMaterial = scene.mMaterials[m];
		ASSERT(assimpMaterial != NULL, "AssetImporter::ProcessMaterials -> scene contains a NULL material.", false);

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
		if(texFound == AI_SUCCESS)diffuseTexture = LoadAITexture(*assimpMaterial, aiTextureType_DIFFUSE, modelPath);
		if(!diffuseTexture.IsValid())
		{
			std::string msg = "AssetImporter::ProcessMaterials -> Could not load diffuse texture.";
			Engine::Instance()->GetErrorManager()->SetAndReportWarning(ModelImporterErrorCodes::MaterialImportFailure, msg);
			materialImportDescriptors.clear();
			return false;
		}

		// loop through each mesh in the scene and check if it uses [material]. If so,
		// create a unique Material object for the mesh and attach it to [materialImportDescriptor]
		//
		// The [materialImportDescriptor] structure describes the unique per-mesh properties we need to be
		// concerned about when creating unique instances of a material for a mesh.
		for(unsigned int i = 0; i < scene.mNumMeshes; i++)
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
					std::string msg = "AssetImporter::ProcessMaterials -> Could not create new Material object.";
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
					bool setupSuccess = SetupMeshSpecificMaterialWithTexture(*assimpMaterial, TextureType::Diffuse, diffuseTexture, i, materialImportDescriptor);
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
	std::string modelDirectory = fileSystem->GetBasePath(modelPath);

	// retrieve the first texture descriptor (at index 0) matching [textureType] from the Assimp material
	texFound = assimpMaterial.GetTexture(textureType, 0, &aiTexturePath);

	ASSERT(texFound == AI_SUCCESS, "ModelImporter::LoadAITexture -> Assimp material does not have desired texture type.", TextureRef::Null(), ModelImporterErrorCodes::AssimpTextureNotFound);

	// build the full path to the texture image as specified by the Assimp material
	std::string texPath = fileSystem->FixupPath(std::string(aiTexturePath.data));
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
bool ModelImporter::SetupMeshSpecificMaterialWithTexture(const aiMaterial& assimpMaterial, TextureType textureType, const TextureRef texture,
		 	 	 	 	 	 	 	 	 	 	 	 	 unsigned int meshIndex, MaterialImportDescriptor& materialImportDesc) const
{
	// get the Assimp material key for textures of type [textureType]
	unsigned int aiTextureKey = ConvertTextureTypeToAITextureKey(textureType);

	// get the name of the shader uniform that handles textures of [textureType]
	std::string textureName = GetBuiltinVariableNameForTextureType(textureType);

	// if we can't find a shader variable for the diffuse texture, then the load of this material has failed
	if(textureName.empty())
	{
		std::string msg ="ModelImporter::SetupImportedMaterialTexture -> Could not locate shader variable for texture.";
		Engine::Instance()->GetErrorManager()->SetAndReportError(ModelImporterErrorCodes::MaterialShaderVariableMatchFailure, msg);
		return false;
	}

	// set the diffuse texture in the material for the mesh specified by [meshIndex]
	materialImportDesc.meshSpecificProperties[meshIndex].material->SetTexture(texture, textureName);

	int mappedIndex;

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
	aiColor4t<float> color;

	// automatically give normals to all materials & meshes (if a mesh doesn't have them by
	// default, they will be calculated)
	LongMaskUtil::SetBit(&flags, (short)ShaderMaterialCharacteristic::VertexNormals);

	// check for a diffuse texture
	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	}

	/*if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::SpecularTextured);
	}

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::Bumped);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::DiffuseColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::SpecularColored);
	}

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::EmissiveColored);
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
	for(unsigned int i = 0; i < scene.mNumMeshes; i++)
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
				LongMaskUtil::SetBit(&meshFlags, (short)ShaderMaterialCharacteristic::VertexColors);
				materialImportDesc.meshSpecificProperties[i].vertexColorsIndex = 0;
			}
			// set mesh specific ShaderMaterialCharacteristic values
			materialImportDesc.meshSpecificProperties[i].shaderProperties = meshFlags;
		}
	}
}

void ModelImporter::SetupVertexBoneMapForRenderer(const aiScene& scene, SkeletonRef skeleton, SkinnedMesh3DRendererRef target, bool reverseVertexOrder) const
{
	for(unsigned int m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != NULL && cMesh->mNumBones > 0)
		{
			VertexBoneMap indexBoneMap(cMesh->mNumVertices, cMesh->mNumVertices);

			bool mapInitSuccess = indexBoneMap.Init();
			if(!mapInitSuccess)
			{
				Debug::PrintError("ModelImporter::SetupVertexBoneMapForRenderer -> Could not initialize index bone map.");
			}

			SetupVertexBoneMapMappingsFromAIMesh(skeleton, *cMesh, indexBoneMap);

			VertexBoneMap * fullBoneMap = ExpandIndexBoneMapping( indexBoneMap, *cMesh, reverseVertexOrder);
			if(fullBoneMap == NULL)
			{
				Debug::PrintError("ModelImporter::SetupVertexBoneMapForRenderer -> Could not create full vertex bone map.");
			}

			target->AddVertexBoneMap(fullBoneMap);
		}
		else
		{
			target->AddVertexBoneMap(NULL);
		}
	}
}

SkeletonRef ModelImporter::LoadSkeleton(const aiScene& scene) const
{
	unsigned int boneCount = CountBones(scene);
	if(boneCount <=0 )
	{
		return SkeletonRef::Null();
	}

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	SkeletonRef target = objectManager->CreateSkeleton(boneCount);
	ASSERT(target.IsValid(),"ModelImporter::LoadSkeleton -> Could not allocate skeleton.",SkeletonRef::Null());

	bool skeletonInitSuccess = target->Init();
	if(!skeletonInitSuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Unable to initialize skeleton.");
		objectManager->DestroySkeleton(target);
		return SkeletonRef::Null();
	}

	unsigned int boneIndex = 0;
	for(unsigned int m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != NULL && cMesh->mNumBones > 0)
		{

			AddMeshBoneMappingsToSkeleton(target, *cMesh, boneIndex);
		}
	}

	bool hierarchysuccess = CreateAndMapNodeHierarchy(target, scene);
	if(!hierarchysuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not create node hierarchy.");
		objectManager->DestroySkeleton(target);
		return SkeletonRef::Null();
	}

	return target;
}

VertexBoneMap * ModelImporter::ExpandIndexBoneMapping(VertexBoneMap& indexBoneMap, const aiMesh& mesh, bool reverseVertexOrder) const
{
	VertexBoneMap * fullBoneMap = new VertexBoneMap(mesh.mNumFaces * 3, mesh.mNumVertices);
	if(fullBoneMap == NULL)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate vertex bone map.");
		return NULL;
	}

	bool mapInitSuccess = fullBoneMap->Init();
	if(!mapInitSuccess)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not initialize vertex bone map.");
		return NULL;
	}

	unsigned fullIndex=0;
	for(unsigned int f = 0; f < mesh.mNumFaces; f++)
	{
		aiFace& face = mesh.mFaces[f];

		int start, end, inc;
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
		for( int i = start; i != end; i+=inc)
		{
			unsigned int vertexIndex = face.mIndices[i];
			fullBoneMap->GetDescriptor(fullIndex)->SetTo(indexBoneMap.GetDescriptor(vertexIndex));
			fullIndex++;
		}
	}

	return fullBoneMap;
}

void ModelImporter::AddMeshBoneMappingsToSkeleton(SkeletonRef skeleton, const aiMesh& mesh, unsigned int& currentBoneIndex) const
{
	ASSERT_RTRN(skeleton.IsValid(), "ModelImporter::AddBoneMappings -> skeleton is invalid.");

	for(unsigned int b = 0; b < mesh.mNumBones; b++)
	{
		aiBone * cBone = mesh.mBones[b];
		if(cBone != NULL)
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

void ModelImporter::SetupVertexBoneMapMappingsFromAIMesh(SkeletonRef skeleton, const aiMesh& mesh, VertexBoneMap& vertexIndexBoneMap) const
{
	ASSERT_RTRN(skeleton.IsValid(), "ModelImporter::AddBoneMappings -> skeleton is invalid.");

	for(unsigned int b = 0; b < mesh.mNumBones; b++)
	{
		aiBone * cBone = mesh.mBones[b];
		if(cBone != NULL)
		{
			std::string boneName = std::string(cBone->mName.C_Str());
			unsigned int boneIndex = skeleton->GetBoneMapping(boneName);

			for(unsigned int w = 0; w < cBone->mNumWeights; w++)
			{
				aiVertexWeight& weightDesc = cBone->mWeights[w];

				unsigned int vertexID = weightDesc.mVertexId;
				float weight = weightDesc.mWeight;

				VertexBoneMap::VertexMappingDescriptor * desc = vertexIndexBoneMap.GetDescriptor(vertexID);
				if(desc != NULL && desc->BoneCount < Constants::MaxBonesPerVertex)
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
	unsigned int boneCount = 0;
	std::unordered_map<std::string, unsigned int> boneCountMap;
	for(unsigned int m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != NULL && cMesh->mNumBones > 0)
		{
			for(unsigned int b = 0; b < cMesh->mNumBones; b++)
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

bool ModelImporter::CreateAndMapNodeHierarchy(SkeletonRef skeleton, const aiScene& scene) const
{
	SceneObjectSkeletonNode * skeletonNode = new SceneObjectSkeletonNode(SceneObjectRef::Null(), -1, "");
	if(skeletonNode == NULL)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate skeleton root node.");
		return false;
	}

	Tree<SkeletonNode*>::TreeNode * lastNode = skeleton->CreateRoot(skeletonNode);
	if(lastNode == NULL)
	{
		Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not create skeleton root node.");
		return false;
	}

	Skeleton * skeletonPtr = skeleton.GetPtr();
	bool success = true;
	TraverseScene(scene, SceneTraverseOrder::PreOrder, [skeletonPtr, lastNode, &success](const aiNode& node) -> bool
	{
		std::string boneName(node.mName.C_Str());
		int mappedBoneIndex = skeletonPtr->GetBoneMapping(boneName);

		SceneObjectSkeletonNode * childSkeletonNode = new SceneObjectSkeletonNode(SceneObjectRef::Null(), mappedBoneIndex, boneName);
		if(childSkeletonNode == NULL)
		{
			Debug::PrintError("ModelImporter::ExpandIndexBoneMapping -> Could not allocate skeleton child node.");
			success  = false;
			return false;
		}

		skeletonPtr->MapNode(boneName, skeletonPtr->GetNodeCount());
		skeletonPtr->AddNodeToList(childSkeletonNode);

		Tree<SkeletonNode*>::TreeNode * childNode = skeletonPtr->AddChild(lastNode, childSkeletonNode);
		if(childNode == NULL)
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

AnimationRef ModelImporter::LoadAnimation (aiAnimation& animation, bool addLoopPadding) const
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	ASSERT(objectManager != NULL,"ModelImporter::LoadAnimation -> EngineObjectManager instance is NULL.", AnimationRef::Null());

	float ticksPerSecond = (float)animation.mTicksPerSecond;

	// adding little extra time to the animation allows for the interpolation between the last
	// and the first frame, which smoothes out looping animations
	// TODO: figure out a better way to do this, possibly a setting for smoothing looped animations
	float loopPadding = ticksPerSecond * .05;
	float durationTicks = (float)animation.mDuration;

	if(addLoopPadding) durationTicks += loopPadding;

	ASSERT(ticksPerSecond > 0, "ModelImporter::LoadAnimation -> tickers per second is 0.", AnimationRef::Null());
	//float duration = durationTicks / ticksPerSecond;

	AnimationRef animationRef = objectManager->CreateAnimation(durationTicks, ticksPerSecond);
	ASSERT(animationRef.IsValid(),"ModelImporter::LoadAnimation -> Unable to create Animation.", AnimationRef::Null());

	bool initSuccess = animationRef->Init(animation.mNumChannels);
	if(!initSuccess)
	{
		objectManager->DestroyAnimation(animationRef);
		Debug::PrintError("ModelImporter::LoadAnimation -> Unable to initialize Animation.");
		return AnimationRef::Null();
	}

	for(unsigned int n = 0; n < animation.mNumChannels; n++)
	{
		aiNodeAnim * nodeAnim = animation.mChannels[n];
		std::string nodeName(nodeAnim->mNodeName.C_Str());

		animationRef->SetChannelName(n,nodeName);

		//int nodeIndex = skeleton->GetNodeMapping(nodeName);
		int nodeIndex = n;
		if(nodeIndex >= 0)
		{
			KeyFrameSet * keyFrameSet = animationRef->GetKeyFrameSet(nodeIndex);
			if(keyFrameSet == NULL)
			{
				objectManager->DestroyAnimation(animationRef);
				std::string msg = std::string("ModelImporter::LoadAnimation -> NULL KeyFrameSet encountered for: ") + nodeName;
				Debug::PrintError(msg);
				return AnimationRef::Null();
			}

			keyFrameSet->Used = true;

			for(unsigned int t = 0; t < nodeAnim->mNumPositionKeys; t++)
			{
				aiVectorKey& vectorKey = *(nodeAnim->mPositionKeys + t);

				TranslationKeyFrame keyFrame;
				keyFrame.NormalizedTime = (float)vectorKey.mTime / durationTicks;
				keyFrame.RealTime = (float)vectorKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (float)vectorKey.mTime;
				keyFrame.Translation.Set(vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z);
				keyFrameSet->TranslationKeyFrames.push_back(keyFrame);
			}

			for(unsigned int s = 0; s < nodeAnim->mNumScalingKeys; s++)
			{

				aiVectorKey& vectorKey = *(nodeAnim->mScalingKeys + s);

				ScaleKeyFrame keyFrame;
				keyFrame.NormalizedTime = (float)vectorKey.mTime / durationTicks;
				keyFrame.RealTime = (float)vectorKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (float)vectorKey.mTime;
				keyFrame.Scale.Set(vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z);
				keyFrameSet->ScaleKeyFrames.push_back(keyFrame);
			}

			for(unsigned int r = 0; r < nodeAnim->mNumRotationKeys; r++)
			{
				aiQuatKey& quatKey = *(nodeAnim->mRotationKeys + r);

				RotationKeyFrame keyFrame;
				keyFrame.NormalizedTime = (float)quatKey.mTime / durationTicks;
				keyFrame.RealTime = (float)quatKey.mTime / ticksPerSecond;
				keyFrame.RealTimeTicks = (float)quatKey.mTime;
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
AnimationRef ModelImporter::LoadAnimation(const std::string& filePath, bool addLoopPadding, bool preserveFBXPivots)
{
	bool initSuccess = InitImporter();
	ASSERT(initSuccess, "ModelImporter::LoadAnimation -> Unable to initialize importer.", AnimationRef::Null());

	const aiScene * scene = LoadAIScene(filePath, preserveFBXPivots);
	ASSERT(scene != NULL, "ModelImporter::LoadAnimation -> Unable to load scene.", AnimationRef::Null());

	ASSERT(scene->mNumAnimations > 0, "ModelImporter::LoadAnimation -> Model does not contain any animations.", AnimationRef::Null());

	// only load the first animation
	AnimationRef animation = LoadAnimation(*(scene->mAnimations[0]), addLoopPadding);
	ASSERT(animation.IsValid(),"ModelImporter::LoadAnimation -> Unable to load Animation.", AnimationRef::Null());

	return animation;
}

void ModelImporter::TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<bool(const aiNode&)> callback) const
{
	if(scene.mRootNode != NULL)
	{
		const aiNode& sceneRef = (const aiNode&)(*(scene.mRootNode));
		if(traverseOrder == SceneTraverseOrder::PreOrder)
			PreOrderTraverseScene(scene, sceneRef, callback);
	}
}

void ModelImporter::PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<bool(const aiNode&)> callback) const
{
	bool doContinue = callback(node);
	if(!doContinue)return;

	for(unsigned int i = 0; i < node.mNumChildren; i++)
	{
		aiNode* childNode = node.mChildren[i];
		if(childNode != NULL)
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
			return mesh.GetUVsTexture0();
		break;
		default:
			return NULL;
		break;
	}

	return NULL;
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

ModelImporter::TextureType ModelImporter::ConvertAITextureKeyToTextureType(int aiTextureKey)
{
	TextureType textureType = TextureType::_None;
	if(aiTextureKey == aiTextureType_SPECULAR)textureType = TextureType::Specular;
	else if(aiTextureKey == aiTextureType_NORMALS)textureType = TextureType::BumpMap;
	else if(aiTextureKey == aiTextureType_DIFFUSE)textureType = TextureType::Diffuse;
	return textureType;

}

int ModelImporter::ConvertTextureTypeToAITextureKey(TextureType textureType)
{
	int aiTextureKey = -1;
	if(textureType == TextureType::Specular)aiTextureKey = aiTextureType_SPECULAR;
	else if(textureType == TextureType::BumpMap)aiTextureKey = aiTextureType_NORMALS;
	else if(textureType == TextureType::Diffuse)aiTextureKey = aiTextureType_DIFFUSE;
	return aiTextureKey;
}

/*
 * Determine if the scale components of [mat] are inverted.
 */
bool ModelImporter::HasInvertedScale(Matrix4x4& mat)
{
	Vector3 trans, scale;
	Quaternion rot;
	mat.Decompose(trans,rot,scale);
	bool invert = false;
	if(scale.x < 0 && scale.y < 0 && scale.z < 0)invert = true;
	else invert = false;
	return invert;
}


