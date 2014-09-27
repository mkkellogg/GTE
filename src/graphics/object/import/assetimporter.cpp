#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include "graphics/stdattributes.h"
#include <IL/il.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "assetimporter.h"
#include "importutil.h"
#include "geometry/matrix4x4.h"
#include "object/engineobjectmanager.h"
#include "object/shadermanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "geometry/sceneobjecttransform.h"
#include "graphics/uv/uv2array.h"
#include "graphics/render/material.h"
#include "graphics/image/rawimage.h"
#include "filesys/filesystem.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "base/longmask.h"
#include "global/global.h"
#include "ui/debug.h"

AssetImporter::AssetImporter()
{

}

AssetImporter::~AssetImporter()
{

}

SceneObject * AssetImporter::LoadModel(const std::string& filePath, float importScale)
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
		return NULL;
	}

	// read the model file in from disk
	scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality  );

	// If the import failed, report it
	if(!scene)
	{
		std::string msg = std::string("AssetImporter::LoadModel -> Could not import file: ") + std::string(importer.GetErrorString());
		Debug::PrintError(msg);
		return NULL;
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModelScene(filePath, *scene, importScale);
}

SceneObject * AssetImporter::ProcessModelScene(const std::string& modelPath, const aiScene& scene, float importScale)
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	std::vector<MaterialImportDescriptor> materialImportDescriptors;
	if(!ProcessMaterials(modelPath, scene, materialImportDescriptors))
	{
		return NULL;
	}

	SceneObject * root = objectManager->CreateSceneObject();
	NULL_CHECK(root,"AssetImporter::ProcessModelScene -> could not create root object", NULL);

	root->SetActive(false);
	Matrix4x4 baseTransform;

	if(scene.mRootNode != NULL)
	{
		RecursiveProcessModelScene(scene, *(scene.mRootNode), importScale, *root, baseTransform, materialImportDescriptors);
	}
	else
	{
		Debug::PrintError("AssetImporter::ProcessModelScene -> Assimp scene root is NULL.");
	}

	return root;
}

void AssetImporter::RecursiveProcessModelScene(const aiScene& scene, const aiNode& node, float scale, SceneObject& current, Matrix4x4& currentTransform,  std::vector<MaterialImportDescriptor>& materialImportDescriptors)
{
	Matrix4x4 mat;

	aiMatrix4x4 matBaseTransformation = node.mTransformation;
	aiMatrix4x4 matScaling;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), matScaling);

	matBaseTransformation = matBaseTransformation * matScaling;
	ImportUtil::ConvertAssimpMatrix(matBaseTransformation,mat);

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();

	Mesh3D * mesh3D = engineObjectManager->CreateMesh3D(node.mNumMeshes);
	NULL_CHECK_RTRN(mesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3D object.");

	bool meshInitSuccess = mesh3D->Init();
	if(!meshInitSuccess)
	{
		Debug::PrintError("AssetImporter::RecursiveProcessModelScene -> Unable to init Mesh3D object.");
		engineObjectManager->DestroyMesh3D(mesh3D);
		return;
	}

	Mesh3DRenderer * meshRenderer = engineObjectManager->CreateMesh3DRenderer();
	if(meshRenderer == NULL)
	{
		Debug::PrintError("AssetImporter::RecursiveProcessModelScene -> Could not create Mesh3DRenderer object.");
		return;
	}

	// create new scene object to hold the Mesh3D object and its renderer
	SceneObject * sceneObject = engineObjectManager->CreateSceneObject();
	NULL_CHECK_RTRN(sceneObject,"AssetImporter::RecursiveProcessModelScene -> Could not create scene object.");

	// update the scene object's local transform
	sceneObject->GetLocalTransform()->SetTo(&mat);

	for (unsigned int n=0; n <  node.mNumMeshes; n++)
	{
		unsigned int sceneMeshIndex = node.mMeshes[n];
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
		Material * material = materialImportDescriptor.meshSpecificProperties[sceneMeshIndex].material;
		NULL_CHECK_RTRN(material,"AssetImporter::RecursiveProcessModelScene -> NULL material encountered.");

		// add the material to the mesh renderer
		meshRenderer->AddMaterial(material);

		// convert Assimp mesh to a Mesh3D object
		SubMesh3D * subMesh3D = ConvertAssimpMesh(*mesh, sceneMeshIndex, materialImportDescriptor);
		NULL_CHECK_RTRN(subMesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.");

		// add the mesh to the newly created scene object
		mesh3D->SetSubMesh(subMesh3D, n);
	}

	sceneObject->SetMesh3D(mesh3D);
	sceneObject->SetMeshRenderer3D(meshRenderer);
	current.AddChild(sceneObject);

	for(unsigned int i=0; i <node.mNumChildren; i++)
	{
		SceneObject * child = engineObjectManager->CreateSceneObject();
		NULL_CHECK_RTRN(child,"AssetImporter::RecursiveProcessModelScene -> Could not create child scene object.");
		current.AddChild(child);
		const aiNode *childNode = node.mChildren[i];
		if(childNode != NULL)RecursiveProcessModelScene(scene, *childNode, scale, *child, mat, materialImportDescriptors);
	}
}

SubMesh3D * AssetImporter::ConvertAssimpMesh(const aiMesh& mesh,  unsigned int meshIndex, MaterialImportDescriptor& materialImportDescriptor)
{
	unsigned int vertexCount = 0;

	// loop through each face in the mesh to get a count of all the vertices
	for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = &(mesh.mFaces[faceIndex]);
		if(face == NULL)
		{
			Debug::PrintError("AssetImporter::ConvertAssimpMesh -> For some reason, mesh has a NULL face!");
			return NULL;
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
	SubMesh3D * mesh3D = engineObjectManager->CreateSubMesh3D(meshAttributes);
	NULL_CHECK(mesh3D,"AssetImporter::ConvertAssimpMesh -> Could not create Mesh3D object.",NULL);

	bool initSuccess = mesh3D->Init(vertexCount);

	// make sure allocation of required number of vertex attributes is successful
	if(!initSuccess)
	{
		engineObjectManager->DestroySubMesh3D(mesh3D);
		Debug::PrintError("AssetImporter::ConvertAssimpMesh -> Could not init mesh.");
		return NULL;
	}

	int vertexComponentIndex = 0;
	int vertexIndex = 0;

	// loop through each face in the mesh and copy relevant vertex attributes
	// into the newly created Mesh3D object
	for (unsigned int faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
	{
		const aiFace* face = &(mesh.mFaces[faceIndex]);

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

	mesh3D->SetNormalsSmoothingThreshold(70);
	mesh3D->Update();

	return mesh3D;
}

bool AssetImporter::ProcessMaterials(const std::string& modelPath, const aiScene& scene, std::vector<MaterialImportDescriptor>& materialImportDescriptors)
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

		Texture * diffuseTexture = NULL;
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
				Shader * loadedShader = engineObjectManager->GetLoadedShader(materialImportDescriptor.meshSpecificProperties[i].shaderProperties);
				if(loadedShader != NULL)
				{
					// create a new Material engine object
					Material * newMaterial = engineObjectManager->CreateMaterial(mtName.C_Str(),loadedShader);
					NULL_CHECK(newMaterial, "AssetImporter::ProcessMaterials -> Could not create new Material object.", false);

					// if there is a diffuse texture, set it and get the appropriate mapping
					// to UV coordinates
					if(diffuseTexture != NULL)
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

void AssetImporter::GetImportDetails(const aiMaterial* mtl, MaterialImportDescriptor& materialImportDesc, const aiScene& scene)
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

UV2Array* AssetImporter::GetMeshUVArrayForShaderMaterialCharacteristic(SubMesh3D& mesh, ShaderMaterialCharacteristic property)
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

StandardUniform AssetImporter::MapShaderMaterialCharacteristicToUniform(ShaderMaterialCharacteristic property)
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

StandardAttribute AssetImporter::MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic property)
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

std::string AssetImporter::GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic property)
{

	StandardUniform uniform = MapShaderMaterialCharacteristicToUniform(property);

	if(uniform != StandardUniform::_None)
	{
		return StandardUniforms::GetUniformName(uniform);
	}

	return "";
}
