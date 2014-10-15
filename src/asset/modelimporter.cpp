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
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "modelimporter.h"
#include "importutil.h"
#include "object/engineobjectmanager.h"
#include "object/shadermanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/bone.h"
#include "graphics/animation/vertexbonemap.h"
#include "graphics/animation/sceneobjectskeletonnode.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
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
#include "global/global.h"
#include "ui/debug.h"

ModelImporter::ModelImporter()
{

}

ModelImporter::~ModelImporter()
{

}

SceneObjectRef ModelImporter::LoadModelDirect(const std::string& filePath, float importScale)
{
	// the global Assimp scene object
	const aiScene* scene = NULL;

	// Create an instance of the Importer class
	Assimp::Importer importer;

	// Check if file exists
	std::ifstream fin(filePath.c_str());
	if(!fin.fail())
	{
		fin.close();
	}
	else
	{
		std::string msg = std::string("AssetImporter::LoadModel -> Could not find file: ") + filePath;
		Debug::PrintError(msg);
		return SceneObjectRef::Null();
	}

	// read the model file in from disk
	scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality  );

	// If the import failed, report it
	if(!scene)
	{
		std::string msg = std::string("AssetImporter::LoadModel -> Could not import file: ") + std::string(importer.GetErrorString());
		Debug::PrintError(msg);
		return SceneObjectRef::Null();
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModelScene(filePath, *scene, importScale);
}

SceneObjectRef ModelImporter::ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale)
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	std::vector<MaterialImportDescriptor> materialImportDescriptors;
	if(!ProcessMaterials(modelPath, scene, materialImportDescriptors))
	{
		return SceneObjectRef::Null();
	}

	SceneObjectRef root = objectManager->CreateSceneObject();
	SHARED_REF_CHECK(root,"AssetImporter::ProcessModelScene -> Could not create root object.", SceneObjectRef::Null());

	root->SetActive(false);
	Matrix4x4 baseTransform;

	if(scene.mRootNode != NULL)
	{
		Skeleton * skeleton = LoadSkeleton(scene);
		std::vector<SceneObjectRef> createdSceneObjects;
		RecursiveProcessModelScene(scene, *(scene.mRootNode), importScale, root,  materialImportDescriptors, skeleton, createdSceneObjects);

		for(unsigned int s = 0; s < createdSceneObjects.size(); s++)
		{
			SkinnedMesh3DRendererRef renderer = createdSceneObjects[s]->GetSkinnedMesh3DRenderer();
			if(renderer.IsValid())
			{
				Skeleton * skeletonClone = skeleton->FullClone();
				if(skeletonClone == NULL)
				{
					Debug::PrintWarning("ModelImporter::ProcessModelScene -> Could not clone scene skeleton.");
					continue;
				}
				renderer->SetSkeleton(skeletonClone);
			}
		}
	}
	else
	{
		Debug::PrintError("AssetImporter::ProcessModelScene -> Assimp scene root is NULL.");
	}

	return root;
}

void ModelImporter::RecursiveProcessModelScene(const aiScene& scene, const aiNode& node, float scale, SceneObjectRef current,   std::vector<MaterialImportDescriptor>& materialImportDescriptors,  Skeleton * skeleton, std::vector<SceneObjectRef>& createdSceneObjects)
{
	Matrix4x4 mat;

	aiMatrix4x4 matBaseTransformation = node.mTransformation;
	aiMatrix4x4 matScaling;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), matScaling);

	matBaseTransformation = matBaseTransformation * matScaling;
	ImportUtil::ConvertAssimpMatrix(matBaseTransformation,mat);

	scale = 1;

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();

	// create new scene object to hold the Mesh3D object and its renderer
	SceneObjectRef sceneObject = engineObjectManager->CreateSceneObject();
	SHARED_REF_CHECK_RTRN(sceneObject,"AssetImporter::RecursiveProcessModelScene -> Could not create scene object.");

	bool hasSkeleton = skeleton != NULL && skeleton->GetBoneCount() ? true : false;
	Mesh3DRenderer * rendererPtr = NULL;
	SkinnedMesh3DRendererRef skinnedMeshRenderer;
	Mesh3DRendererRef meshRenderer;

	//printf("node: %s\n",node.mName.C_Str());

	if(node.mNumMeshes > 0)
	{
		Mesh3DRef mesh3D = engineObjectManager->CreateMesh3D(node.mNumMeshes);
		SHARED_REF_CHECK_RTRN(mesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3D object.");

		bool meshInitSuccess = mesh3D->Init();
		if(!meshInitSuccess)
		{
			Debug::PrintError("AssetImporter::RecursiveProcessModelScene -> Unable to init Mesh3D object.");
			return;
		}

		if(hasSkeleton)
		{
			skinnedMeshRenderer = engineObjectManager->CreateSkinnedMesh3DRenderer();
			SHARED_REF_CHECK_RTRN(skinnedMeshRenderer,"AssetImporter::RecursiveProcessModelScene -> Could not create SkinnedMesh3DRenderer object.");
			rendererPtr = (Mesh3DRenderer*)skinnedMeshRenderer.GetPtr();
		}
		else
		{
			meshRenderer = engineObjectManager->CreateMesh3DRenderer();
			SHARED_REF_CHECK_RTRN(meshRenderer,"AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3DRenderer object.");
			rendererPtr = meshRenderer.GetPtr();
		}

		std::unordered_map<unsigned int, unsigned int> subMeshInmdexToVertexBoneMap;
		for (unsigned int n=0; n < node.mNumMeshes; n++)
		{
			unsigned int sceneMeshIndex = node.mMeshes[n];
			subMeshInmdexToVertexBoneMap[n] = sceneMeshIndex;
			const aiMesh* mesh = scene.mMeshes[sceneMeshIndex];
			if(mesh == NULL)
			{
				std::string msg("AssetImporter::RecursiveProcessModelScene -> Mesh is NULL at index: ");
				msg += std::to_string(sceneMeshIndex);
				Debug::PrintError(msg);
				return;
			}

			int materialIndex = mesh->mMaterialIndex;
			MaterialImportDescriptor& materialImportDescriptor = materialImportDescriptors[materialIndex];
			MaterialRef material = materialImportDescriptor.meshSpecificProperties[sceneMeshIndex].material;
			SHARED_REF_CHECK_RTRN(material,"AssetImporter::RecursiveProcessModelScene -> NULL Material object encountered.");

			// add the material to the mesh renderer
			rendererPtr->AddMaterial(material);

			// convert Assimp mesh to a Mesh3D object
			SubMesh3DRef subMesh3D = ConvertAssimpMesh(*mesh, sceneMeshIndex, materialImportDescriptor);
			SHARED_REF_CHECK_RTRN(subMesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.");

			// add the mesh to the newly created scene object
			mesh3D->SetSubMesh(subMesh3D, n);
		}

		if(hasSkeleton)
		{
			for (unsigned int n=0; n < node.mNumMeshes; n++)
			{
				skinnedMeshRenderer->MapSubMeshToVertexBoneMap(n, subMeshInmdexToVertexBoneMap[n]);
			}

			sceneObject->SetSkinnedMesh3DRenderer(skinnedMeshRenderer);
			skinnedMeshRenderer->SetMesh(mesh3D);
		}
		else
		{
			sceneObject->SetMesh3DRenderer(meshRenderer);
			sceneObject->SetMesh3D(mesh3D);
		}
	}

	if(hasSkeleton)
	{
		if(node.mName.C_Str() != NULL)
		{
			std::string boneName(node.mName.C_Str());
			int boneMapping = skeleton->GetBoneMapping(boneName);
			if(boneMapping>=0)
			{
				Bone * bone = skeleton->GetBone(boneMapping);
				if(bone != NULL)
				{
					SkeletonNode * skNode = bone->Node;
					{
						// if this skeleton node has a SceneObject target, then set it to [sceneObject]
						SceneObjectSkeletonNode *soskNode = dynamic_cast<SceneObjectSkeletonNode*>(skNode);
						if(soskNode != NULL)
						{
							//printf("targeting: %s\n",node.mName.C_Str());
							//const float * data = sceneObject->GetLocalTransform().GetMatrix()->GetDataPtr();
							//printf("mapped: %s [%f,%f,%f,%f]\n",node.mName.C_Str(), data[0], data[5], data[10], data[15]);
							soskNode->SetTarget(sceneObject);
						}
					}
				}
			}
		}
	}

	// update the scene object's local transform
	std::string name(node.mName.C_Str());
	sceneObject->SetName(name);
	sceneObject->GetLocalTransform().SetTo(&mat);
	current->AddChild(sceneObject);
	createdSceneObjects.push_back(sceneObject);

	for(unsigned int i=0; i <node.mNumChildren; i++)
	{
		//SceneObjectRef child = engineObjectManager->CreateSceneObject();
		//SHARED_REF_CHECK_RTRN(child,"AssetImporter::RecursiveProcessModelScene -> Could not create child object.");

		//current->AddChild(child);
		const aiNode *childNode = node.mChildren[i];
		if(childNode != NULL)RecursiveProcessModelScene(scene, *childNode, scale, sceneObject, materialImportDescriptors, skeleton, createdSceneObjects);
	}
}

SubMesh3DRef ModelImporter::ConvertAssimpMesh(const aiMesh& mesh,  unsigned int meshIndex, MaterialImportDescriptor& materialImportDescriptor)
{
	unsigned int vertexCount = 0;

	// loop through each face in the mesh to get a count of all the vertices
	for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = mesh.mFaces + faceIndex;
		if(face == NULL)
		{
			Debug::PrintError("AssetImporter::ConvertAssimpMesh -> For some reason, mesh has a NULL face!");
			return SubMesh3DRef::Null();
		}
		vertexCount += face->mNumIndices;
	}

	// create a set of standard attributes that will dictate the standard attributes
	// to be used by the Mesh3D object created by this function.
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();

	// all meshes must have vertex positions
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

	int diffuseTextureUVIndex = -1;
	// update the StandardAttributeSet to contain appropriate attributes (UV coords) for a diffuse texture
	if(materialImportDescriptor.UVMappingHasKey(ShaderMaterialCharacteristic::DiffuseTextured))
	{
		StandardAttributes::AddAttribute(&meshAttributes, MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic::DiffuseTextured));
		diffuseTextureUVIndex = materialImportDescriptor.uvMapping[ShaderMaterialCharacteristic::DiffuseTextured];
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

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();

	// create Mesh3D object with the constructed StandardAttributeSet
	SubMesh3DRef mesh3D = engineObjectManager->CreateSubMesh3D(meshAttributes);
	SHARED_REF_CHECK(mesh3D,"AssetImporter::ConvertAssimpMesh -> Could not create Mesh3D object.", SubMesh3DRef::Null());

	bool initSuccess = mesh3D->Init(vertexCount);

	// make sure allocation of required number of vertex attributes is successful
	if(!initSuccess)
	{
		engineObjectManager->DestroySubMesh3D(mesh3D);
		Debug::PrintError("AssetImporter::ConvertAssimpMesh -> Could not init mesh.");
		return SubMesh3DRef::Null();
	}

	int vertexComponentIndex = 0;
	int vertexIndex = 0;

	// loop through each face in the mesh and copy relevant vertex attributes
	// into the newly created Mesh3D object
	for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = mesh.mFaces + faceIndex;

		for( int i = face->mNumIndices-1; i >=0; i--)
		{
			int vIndex = face->mIndices[i];

			aiVector3D srcPosition = mesh.mVertices[vIndex];

			// copy vertex position
			mesh3D->GetPostions()->GetPoint(vertexIndex)->Set(srcPosition.x,srcPosition.y,srcPosition.z);

			// copy mesh normals
			if(mesh.mNormals != NULL)
			{
				aiVector3D& srcNormal = mesh.mNormals[vIndex];
				mesh3D->GetNormals()->GetVector(vertexIndex)->Set(srcNormal.x,srcNormal.y,srcNormal.z);
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
				if(materialImportDescriptor.invertVCoords)uvs->GetCoordinate(vertexIndex)->Set(mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].x, 1-mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].y);
				else uvs->GetCoordinate(vertexIndex)->Set(mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].x, mesh.mTextureCoords[diffuseTextureUVIndex][vIndex].y);
			}

			vertexComponentIndex+=3;
			vertexIndex++;
		}
	}

	mesh3D->SetNormalsSmoothingThreshold(80);
	mesh3D->Update();

	return mesh3D;
}

bool ModelImporter::ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors)
{
	// TODO: Implement support for embedded textures
	if (scene.HasTextures())
	{
		Debug::PrintError("AssetImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();
	FileSystem * fileSystem = FileSystem::Instance();
	std::string basepath = fileSystem->GetBasePath(modelPath);

	// loop through each scene material and extract relevant textures and
	// other properties and create a MaterialDescriptor object that will hold those
	// properties and all corresponding Material objects
	for (unsigned int m=0; m < scene.mNumMaterials; m++)
	{
		aiReturn texFound = AI_SUCCESS;
		aiString aiTexturePath;

		aiMaterial * material = scene.mMaterials[m];
		NULL_CHECK(material, "AssetImporter::ProcessMaterials -> scene contains a NULL material.", false);

		aiString mtName;
		material->Get(AI_MATKEY_NAME,mtName);

		// build an import descriptor for this material
		MaterialImportDescriptor materialImportDescriptor;
		GetImportDetails(material, materialImportDescriptor, scene);

		TextureRef diffuseTexture;
	//	Texture * bumpTexture = NULL;

		// get diffuse texture (for now support only 1)
		texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		std::string filename;
		if(texFound == AI_SUCCESS)
		{
			// load & create diffuse texture
			std::string texPath = fileSystem->FixupPath(std::string(aiTexturePath.data));
			filename = fileSystem->ConcatenatePaths(basepath, texPath);

			TextureAttributes texAttributes;
			texAttributes.FilterMode = TextureFilter::TriLinear;
			texAttributes.MipMapLevel = 4;
			diffuseTexture = engineObjectManager->CreateTexture(filename.c_str(),texAttributes);
		}

		// loop through each mesh in the scene and check if it uses [material]. If so,
		// create a unique Material object for the mesh and attach it to [materialImportDescriptor]
		for(unsigned int i = 0; i < scene.mNumMeshes; i++)
		{
			if(materialImportDescriptor.UsedByMesh(i))
			{
				// see if we can match a loaded shader to the properties of this material
				// if we can't find one...well we can't really load this material
				ShaderRef loadedShader = engineObjectManager->GetLoadedShader(materialImportDescriptor.meshSpecificProperties[i].shaderProperties);
				if(loadedShader.IsValid())
				{
					// create a new Material engine object
					MaterialRef newMaterial = engineObjectManager->CreateMaterial(mtName.C_Str(),loadedShader);
					SHARED_REF_CHECK(newMaterial, "AssetImporter::ProcessMaterials -> Could not create new Material object.", false);

					// if there is a diffuse texture, set it and get the appropriate mapping
					// to UV coordinates
					if(diffuseTexture.IsValid())
					{
						// get the name of the shader uniform that handles diffuse textures
						std::string diffuseTextureName = GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic::DiffuseTextured);
						if(!diffuseTextureName.empty())newMaterial->SetTexture(diffuseTexture, diffuseTextureName);

						int mappedIndex;

						// get the assimp UV channel for this diffuse texture. the mapping will be used later when
						// importing meshes
						if(AI_SUCCESS==aiGetMaterialInteger(material,AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE,0),&mappedIndex))
							materialImportDescriptor.uvMapping[ShaderMaterialCharacteristic::DiffuseTextured] = mappedIndex;
						else
							materialImportDescriptor.uvMapping[ShaderMaterialCharacteristic::DiffuseTextured] = 0;
					}

					// add new material and corresponding import descriptor to list for this model
					materialImportDescriptor.meshSpecificProperties[i].material = newMaterial;
				}
				else
				{
					std::string msg = "Could not find loaded shader for: ";
					msg += std::bitset<64>(materialImportDescriptor.meshSpecificProperties[i].shaderProperties).to_string();
					Debug::PrintError(msg);
					return false;
				}
			}
		}
		materialImportDescriptors.push_back(materialImportDescriptor);
	}

	return true;
}

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
	// active ShaderMaterialCharacteristic values for that mesh
	for(unsigned int i = 0; i < scene.mNumMeshes; i++)
	{
		// copy the existing set of ShaderMaterialCharacteristic values
		LongMask meshFlags = flags;
		// get mesh
		const aiMesh * mesh = scene.mMeshes[i];
		unsigned int materialIndex = mesh->mMaterialIndex;

		// compare current meshes material to [mtl]
		if(scene.mMaterials[materialIndex] == mtl)
		{
			unsigned int meshSpecificIndex = i;
			// for now only support one set of vertex colors, and look at index 0 for it
			if(mesh->HasVertexColors(0))
			{
				LongMaskUtil::SetBit(&meshFlags, (short)ShaderMaterialCharacteristic::VertexColors);
				materialImportDesc.meshSpecificProperties[meshSpecificIndex].vertexColorsIndex = 0;
			}
			// set mesh specific ShaderMaterialCharacteristic values
			materialImportDesc.meshSpecificProperties[meshSpecificIndex].shaderProperties = meshFlags;
		}
	}
}

Skeleton * ModelImporter::LoadSkeleton(const aiScene& scene)
{
	unsigned int boneCount = CountBones(scene);
	if(boneCount <=0 )
	{
		return NULL;
	}

	Skeleton * target = new Skeleton(boneCount);
	NULL_CHECK(target,"ModelImporter::LoadSkeleton -> Could not allocate skeleton.",NULL);

	bool skeletonInitSuccess = target->Init();
	if(!skeletonInitSuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Unable to initialize skeleton.");
		return NULL;
	}

	unsigned int boneIndex = 0;
	for(unsigned int m = 0; m < scene.mNumMeshes; m++)
	{
		aiMesh * cMesh = scene.mMeshes[m];
		if( cMesh != NULL && cMesh->mNumBones > 0)
		{
			VertexBoneMap indexBoneMap(cMesh->mNumVertices);
			bool mapInitSuccess = indexBoneMap.Init();
			if(!mapInitSuccess)
			{
				Debug::PrintError("ModelImporter::LoadSkeleton -> Could not initialize index bone map.");
				delete target;
				return NULL;
			}

			AddBoneMappings(*target, *cMesh, boneIndex, indexBoneMap);
			VertexBoneMap * fullBoneMap = ExpandIndexBoneMapping(*target, indexBoneMap, *cMesh);
			if(fullBoneMap == NULL)
			{
				delete target;
				return NULL;
			}

			target->AddVertexBoneMap(fullBoneMap);
		}
		else
		{
			target->AddVertexBoneMap(NULL);
		}
	}

	bool hierarchysuccess = CreateAndMapNodeHierarchy(target, scene);
	if(!hierarchysuccess)
	{
		delete target;
		return NULL;
	}

	return target;
}

VertexBoneMap * ModelImporter::ExpandIndexBoneMapping(Skeleton& skeleton, VertexBoneMap& indexBoneMap, const aiMesh& mesh)
{
	VertexBoneMap * fullBoneMap = new VertexBoneMap(mesh.mNumFaces * 3);
	if(fullBoneMap == NULL)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not allocate vertexBoneMap.");
		return NULL;
	}

	bool mapInitSuccess = fullBoneMap->Init();
	if(!mapInitSuccess)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not initialize vertex bone map.");
		return NULL;
	}

	unsigned fullIndex=0;
	for(unsigned int f = 0; f < mesh.mNumFaces; f++)
	{
		aiFace& face = mesh.mFaces[f];
		for(unsigned int i = 0; i < face.mNumIndices; i++)
		{
			unsigned int vertexIndex = face.mIndices[i];
			fullBoneMap->GetDescriptor(fullIndex)->SetTo(indexBoneMap.GetDescriptor(vertexIndex));
			fullIndex++;
		}
	}

	return fullBoneMap;
}

void ModelImporter::AddBoneMappings(Skeleton& skeleton, const aiMesh& mesh, unsigned int& currentBoneIndex, VertexBoneMap& vertexIndexBoneMap)
{
	for(unsigned int b = 0; b < mesh.mNumBones; b++)
	{
		aiBone * cBone = mesh.mBones[b];
		if(cBone != NULL)
		{
			std::string boneName = std::string(cBone->mName.C_Str());
			if(skeleton.GetBoneMapping(boneName) == -1)
			{
				skeleton.MapBone(boneName, currentBoneIndex);

				for(unsigned int w = 0; w < cBone->mNumWeights; w++)
				{
					aiVertexWeight& weightDesc = cBone->mWeights[w];

					unsigned int vertexID = weightDesc.mVertexId;
					float weight = weightDesc.mWeight;

					VertexBoneMap::VertexMappingDescriptor * desc = vertexIndexBoneMap.GetDescriptor(vertexID);
					if(desc != NULL && desc->BoneCount < Constants::MaxBonesPerVertex)
					{
						desc->BoneIndex[desc->BoneCount] = currentBoneIndex;
						desc->Weight[desc->BoneCount] = weight;
						desc->BoneCount++;
					}
				}

				Matrix4x4 offsetMatrix;
				ImportUtil::ConvertAssimpMatrix(cBone->mOffsetMatrix, offsetMatrix);

				skeleton.GetBone(currentBoneIndex)->Name = boneName;
				skeleton.GetBone(currentBoneIndex)->ID = currentBoneIndex;
				skeleton.GetBone(currentBoneIndex)->OffsetMatrix.SetTo(&offsetMatrix);

				currentBoneIndex++;
			}
		}
	}
}

unsigned ModelImporter::CountBones(const aiScene& scene)
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

bool ModelImporter::CreateAndMapNodeHierarchy(Skeleton * skeleton, const aiScene& scene)
{
	SceneObjectSkeletonNode * skeletonNode = new SceneObjectSkeletonNode(SceneObjectRef::Null(), -1);
	if(skeletonNode == NULL)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not allocate skeleton root node.");
		return false;
	}

	Tree<SkeletonNode*>::TreeNode * lastNode = skeleton->CreateRoot(skeletonNode);
	if(lastNode == NULL)
	{
		Debug::PrintError("ModelImporter::LoadSkeleton -> Could not create skeleton root node.");
		return false;
	}

	bool success = true;
	TraverseScene(scene, SceneTraverseOrder::PreOrder, [skeleton, lastNode, &success](const aiNode& node) -> bool
	{
		std::string boneName(node.mName.C_Str());
		int mappedBoneIndex = skeleton->GetBoneMapping(boneName);

		SceneObjectSkeletonNode * childSkeletonNode = new SceneObjectSkeletonNode(SceneObjectRef::Null(), mappedBoneIndex);
		if(childSkeletonNode == NULL)
		{
			Debug::PrintError("ModelImporter::LoadSkeleton -> Could not allocate skeleton child node.");
			success  = false;
			return false;
		}

		Tree<SkeletonNode*>::TreeNode * childNode = skeleton->AddChild(lastNode, childSkeletonNode);
		if(childNode == NULL)
		{
			Debug::PrintError("ModelImporter::LoadSkeleton -> Could not create skeleton child node.");
			success  = false;
			return false;
		}

		if(mappedBoneIndex >= 0)
		{
			Bone * bone = skeleton->GetBone(mappedBoneIndex);
			bone->Node = childSkeletonNode;
		}

		return true;
	});

	return success;
}

void ModelImporter::TraverseScene(const aiScene& scene, SceneTraverseOrder traverseOrder, std::function<bool(const aiNode&)> callback)
{
	if(scene.mRootNode != NULL)
	{
		const aiNode& sceneRef = (const aiNode&)(*(scene.mRootNode));
		if(traverseOrder == SceneTraverseOrder::PreOrder)
			PreOrderTraverseScene(scene, sceneRef, callback);
	}
}

void ModelImporter::PreOrderTraverseScene(const aiScene& scene, const aiNode& node, std::function<bool(const aiNode&)> callback)
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

std::string ModelImporter::GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property)
{

	StandardUniform uniform = MapShaderMaterialCharacteristicToUniform(property);

	if(uniform != StandardUniform::_None)
	{
		return StandardUniforms::GetUniformName(uniform);
	}

	return "";
}

