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
	return ProcessModelScene(filePath, scene, importScale);
}

SceneObject * AssetImporter::ProcessModelScene(const std::string& modelPath, const aiScene* scene, float importScale)
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	std::vector<Material *> materials;
	std::vector<MaterialImportDescriptor> materialImportDescriptors;
	if(!ProcessMaterials(modelPath, scene, materials, materialImportDescriptors))
	{
		return NULL;
	}

	SceneObject * root = objectManager->CreateSceneObject();
	NULL_CHECK(root,"AssetImporter::ProcessModelScene -> could not create root object", NULL);

	root->SetActive(false);
	Matrix4x4 baseTransform;
	RecursiveProcessModelScene(scene, scene->mRootNode, importScale, root, &baseTransform, materials, materialImportDescriptors);

	return root;
}

void AssetImporter::RecursiveProcessModelScene(const aiScene *scene, const aiNode* nd, float scale, SceneObject * current, Matrix4x4 * currentTransform, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor>& materialImportDescriptors)
{
	Matrix4x4 mat;

	aiMatrix4x4 m = nd->mTransformation;
	aiMatrix4x4 m2;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);

	m = m * m2;
	ImportUtil::ConvertAssimpMatrix(&m,&mat);

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();

	unsigned int meshCount = nd->mNumMeshes;
	for (unsigned int n=0; n < meshCount; n++)
	{
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		int materialIndex = mesh->mMaterialIndex;
		Material * material = materials[materialIndex];
		NULL_CHECK_RTRN(material,"AssetImporter::RecursiveProcessModelScene -> NULL material encountered.");

		MaterialImportDescriptor& materialImportDescriptor = materialImportDescriptors[materialIndex];

		Mesh3D * mesh3D = ConvertAssimpMesh(mesh, material, &materialImportDescriptor);
		NULL_CHECK_RTRN(mesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.");

		SceneObject * sceneObject = engineObjectManager->CreateSceneObject();
		NULL_CHECK_RTRN(sceneObject,"AssetImporter::RecursiveProcessModelScene -> Could not create scene object.");

		Mesh3DRenderer * meshRenderer = engineObjectManager->CreateMesh3DRenderer();
		NULL_CHECK_RTRN(meshRenderer,"AssetImporter::RecursiveProcessModelScene -> Could not create mesh renderer.");

		meshRenderer->SetMaterial(material);

		sceneObject->SetMesh3D(mesh3D);
		sceneObject->SetMeshRenderer3D(meshRenderer);
		sceneObject->GetLocalTransform()->SetTo(&mat);
		current->AddChild(sceneObject);
	}

	for(unsigned int i=0; i <nd->mNumChildren; i++)
	{
		SceneObject * child = engineObjectManager->CreateSceneObject();
		NULL_CHECK_RTRN(child,"AssetImporter::RecursiveProcessModelScene -> Could not create child scene object.");
		current->AddChild(child);

		const aiNode *node = nd->mChildren[i];
		if(node != NULL)RecursiveProcessModelScene(scene, node, scale, child, &mat, materials, materialImportDescriptors);
	}
}

Mesh3D * AssetImporter::ConvertAssimpMesh(const aiMesh* mesh, Material * material, MaterialImportDescriptor * materialImportDescriptor)
{
	unsigned int vertexCount = 0;
	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
	{
		const aiFace* face = &mesh->mFaces[faceIndex];
		vertexCount += face->mNumIndices;
	}

	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

	int diffuseTextureUVIndex = -1;
	if(materialImportDescriptor->UVMappingHasKey(ShaderMaterialCharacteristic::DiffuseTextured))
	{
		StandardAttributes::AddAttribute(&meshAttributes, MapShaderMaterialCharacteristicToAttribute(ShaderMaterialCharacteristic::DiffuseTextured));
		diffuseTextureUVIndex = materialImportDescriptor->uvMapping[ShaderMaterialCharacteristic::DiffuseTextured];
	}

	if(mesh->mNormals != NULL)
	{
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	}

	if(mesh->HasVertexColors(0))
	{
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Color);
	}

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();
	Mesh3D * mesh3D = engineObjectManager->CreateMesh3D(meshAttributes);

	NULL_CHECK(mesh3D,"AssetImporter::ConvertAssimpMesh -> Could not create Mesh3D object.",NULL);

	bool initSuccess = mesh3D->Init(vertexCount);

	if(!initSuccess)
	{
		engineObjectManager->DestroyMesh3D(mesh3D);
		Debug::PrintError("AssetImporter::ConvertAssimpMesh -> Could not init mesh.");
		return NULL;
	}

	int vertexComponentIndex = 0;
	int vertexIndex = 0;
	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
	{
		const aiFace* face = &mesh->mFaces[faceIndex];

		for( int i = face->mNumIndices-1; i >=0; i--)
		{
			int vIndex = face->mIndices[i];

			aiVector3D srcPosition = mesh->mVertices[vIndex];

			mesh3D->GetPostions()->GetPoint(vertexIndex)->Set(srcPosition.x,srcPosition.y,srcPosition.z);
			if(mesh->mNormals != NULL)
			{
				aiVector3D srcNormal = mesh->mNormals[vIndex];
				mesh3D->GetNormals()->GetVector(vertexIndex)->Set(srcNormal.x,srcNormal.y,srcNormal.z);
			}

			if(mesh->HasVertexColors(0))
			{
				mesh3D->GetColors()->GetColor(vertexIndex)->Set(mesh->mColors[0]->r,mesh->mColors[0]->g,mesh->mColors[0]->b,mesh->mColors[0]->a);
			}

			if(diffuseTextureUVIndex >= 0)
			{
				UV2Array *uvs = GetMeshUVArrayForShaderMaterialCharacteristic(mesh3D,ShaderMaterialCharacteristic::DiffuseTextured);
				if(materialImportDescriptor->invertVCoords)uvs->GetCoordinate(vertexIndex)->Set(mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].x, 1-mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].y);
				else uvs->GetCoordinate(vertexIndex)->Set(mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].x, mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].y);
			}

			vertexComponentIndex+=3;
			vertexIndex++;
		}
	}

	mesh3D->SetNormalsSmoothingThreshold(70);
	mesh3D->Update();

	return mesh3D;
}

bool AssetImporter::ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor>& materialImportDescriptors)
{
	NULL_CHECK(scene,"AssetImporter::ProcessMaterials -> scene is NULL.", false);

	// TODO: Implement support for embedded textures
	if (scene->HasTextures())
	{
		Debug::PrintError("AssetImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();
	FileSystem * fileSystem = FileSystem::Instance();
	std::string basepath = fileSystem->GetBasePath(modelPath);

	// loop through each scene material and extract relevant textures and
	// other properties and create an equivalent Material engine object
	for (unsigned int m=0; m < scene->mNumMaterials; m++)
	{
		aiReturn texFound = AI_SUCCESS;
		aiString aiTexturePath;

		aiMaterial * material = scene->mMaterials[m];
		aiString mtName;
		material->Get(AI_MATKEY_NAME,mtName);
		LongMask shaderProperties = GetImportFlags(material);

		Texture * diffuseTexture = NULL;
	//	Texture * bumpTexture = NULL;

		// get diffuse texture (for now support only 1)
		texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		std::string filename;
		if(texFound == AI_SUCCESS)
		{
			std::string texPath = fileSystem->FixupPath(std::string(aiTexturePath.data));
			filename = fileSystem->ConcatenatePaths(basepath, texPath);
			TextureAttributes texAttributes;
			texAttributes.FilterMode = TextureFilter::TriLinear;
			texAttributes.MipMapLevel = 4;
			diffuseTexture = engineObjectManager->CreateTexture(filename.c_str(),texAttributes);
		}

		// see if we can match a loaded shader to the properties of this material
		// if we can't find one...well we can't really load this material
		Shader * loadedShader = engineObjectManager->GetLoadedShader(shaderProperties);
		if(loadedShader != NULL)
		{
			// build an import descriptor for this material
			MaterialImportDescriptor materialImportDescriptor;

			// create a new Material engine object
			Material * newMaterial = engineObjectManager->CreateMaterial(mtName.C_Str(),loadedShader);

			// if there is a diffuse texture, set it and get the appropriate mapping
			// to UV coordinates
			if(diffuseTexture != NULL)
			{
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
			materials.push_back(newMaterial);
			materialImportDescriptors.push_back(materialImportDescriptor);
		}
		else
		{
			std::string msg = "Could not find loaded shader for: ";
			msg += std::bitset<64>(shaderProperties).to_string();
			Debug::PrintError(msg);
			return false;
		}
	}

	return true;
}

void AssetImporter::UpdateImportFlags(LongMask * flags, const aiMesh* mesh)
{
	if(mesh->HasVertexColors(0))
	{
		LongMaskUtil::SetBit(flags, (short)ShaderMaterialCharacteristic::VertexColors);
	}
}

LongMask AssetImporter::GetImportFlags(const aiMaterial * mtl)
{
	LongMask flags = LongMaskUtil::CreateLongMask();
	aiString path;
	aiColor4t<float> color;

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	}



	/*if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path))
	{
		LongMaskUtil::SetBit(&flags, (short)ShaderMaterialProperty::SpecularTextured);
	}*/

	/*if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path))
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

	return flags;
}

UV2Array* AssetImporter::GetMeshUVArrayForShaderMaterialCharacteristic(Mesh3D * mesh, ShaderMaterialCharacteristic property)
{
	switch(property)
	{
		case ShaderMaterialCharacteristic::DiffuseTextured:
			return mesh->GetUVsTexture0();
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
