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
#include "assimp/Importer.hpp" //OO version Header!
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
		return NULL;
	}

	scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality  );
	// If the import failed, report it

	if( !scene)
	{
		Debug::PrintError(importer.GetErrorString());
		return NULL;
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModelScene(filePath, scene, importScale);
}

SceneObject * AssetImporter::ProcessModelScene(const std::string& modelPath, const aiScene* scene, float importScale)
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	std::vector<Material *> materials;
	std::vector<MaterialImportDescriptor *> textureUVMaps;
	ProcessMaterials(modelPath, scene, materials, textureUVMaps);

	SceneObject * root = objectManager->CreateSceneObject();
	NULL_CHECK(root,"AssetImporter::ProcessModelScene -> could not create root object", NULL);

	root->SetActive(false);
	Matrix4x4 baseTransform;
	RecursiveProcessModelScene(scene, scene->mRootNode, importScale, root, &baseTransform, materials, textureUVMaps);

	return root;
}

void AssetImporter::RecursiveProcessModelScene(const aiScene *scene, const aiNode* nd, float scale, SceneObject * current, Matrix4x4 * currentTransform, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor *>& materialImportDescriptors)
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

		MaterialImportDescriptor * textureUVMap = materialImportDescriptors[materialIndex];
		NULL_CHECK_RTRN(textureUVMap,"AssetImporter::RecursiveProcessModelScene -> NULL textureUVMap encountered.");

		Mesh3D * mesh3D = ConvertAssimpMesh(mesh, material, textureUVMap);
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
	unsigned int faceCount = 0;
	unsigned int vertexCount = 0;

	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
	{
		const aiFace* face = &mesh->mFaces[faceIndex];
		vertexCount += face->mNumIndices;
		faceCount++;
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

	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Color);

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

		/*GLenum face_mode;
		switch(face->mNumIndices)
		{
			case 1: face_mode = GL_POINTS; break;
			case 2: face_mode = GL_LINES; break;
			case 3: face_mode = GL_TRIANGLES; break;
			default: face_mode = GL_POLYGON; break;
		}
		glBegin(face_mode);*/

		for( int i = face->mNumIndices-1; i >=0; i--)	// go through all vertices in face
		{
			int vIndex = face->mIndices[i];	// get group index for current index

			//if(mesh->mColors[0] != NULL)
			//Color4f(&mesh->mColors[0][vertexIndex]);

			/*if(mesh->mNormals != NULL)
			if(mesh->HasTextureCoords(0))	//HasTextureCoords(texture_coordinates_set)
			{
				glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
			}*/

			//glNormal3fv(&mesh->mNormals[vertexIndex].x);
			//glVertex3fv(&mesh->mVertices[vertexIndex].x);

			/*vertices[vertexComponentIndex] = mesh->mVertices[vIndex].x;
			vertices[vertexComponentIndex+1] = mesh->mVertices[vIndex].y;
			vertices[vertexComponentIndex+2] = mesh->mVertices[vIndex].z;*/

			aiVector3D srcPosition = mesh->mVertices[vIndex];

			mesh3D->GetPostions()->GetPoint(vertexIndex)->Set(srcPosition.x,srcPosition.y,srcPosition.z);
			if(mesh->mNormals != NULL)
			{
				aiVector3D srcNormal = mesh->mNormals[vIndex];
				mesh3D->GetNormals()->GetVector(vertexIndex)->Set(srcNormal.x,srcNormal.y,srcNormal.z);
			}
			mesh3D->GetColors()->GetColor(vertexIndex)->Set(1,1,1,1);

			if(diffuseTextureUVIndex >= 0)
			{
				UV2Array *uvs = GetMeshUVArrayForShaderMaterialCharacteristic(mesh3D,ShaderMaterialCharacteristic::DiffuseTextured);
				if(materialImportDescriptor->invertVCoords)uvs->GetCoordinate(vertexIndex)->Set(mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].x, 1-mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].y);
				else uvs->GetCoordinate(vertexIndex)->Set(mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].x, mesh->mTextureCoords[diffuseTextureUVIndex][vIndex].y);
			}

			vertexComponentIndex+=3;
			vertexIndex++;
		}

		//glEnd();
	}

	mesh3D->SetNormalsSmoothingThreshold(70);
	mesh3D->Update();

	return mesh3D;
}

bool AssetImporter::ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials, std::vector<MaterialImportDescriptor *>& materialImportDescriptors)
{
	if (scene->HasTextures())
	{
		Debug::PrintError("AssetImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	NULL_CHECK(scene,"AssetImporter::ProcessMaterials -> scene is NULL.", false);

	//int textureCount = 0;
	std::vector<std::string> texturePaths;

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();
	FileSystem * fileSystem = FileSystem::Instance();
	std::string basepath = fileSystem->GetBasePath(modelPath);

	//printf("num materials: %d\n", scene->mNumMaterials);


	// count textures in scene and get file paths
	for (unsigned int m=0; m < scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		aiString path;	// filename

		aiMaterial * material = scene->mMaterials[m];
		LongMask shaderProperties = GetImportFlags(material);
		Texture * diffuseTexture = NULL;
	//	Texture * bumpTexture = NULL;

		texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		std::string filename;
		if(texFound == AI_SUCCESS)
		{
			aiString mtName;
			material->Get(AI_MATKEY_NAME,mtName);
			std::string texPath = fileSystem->FixupPath(std::string(path.data));

			//printf("material name: %s, path: %s\n", mtName.C_Str(), texPath.c_str());

			filename = fileSystem->ConcatenatePaths(basepath, texPath);
			TextureAttributes texAttributes;
			texAttributes.FilterMode = TextureFilter::TriLinear;
			texAttributes.MipMapLevel = 4;
			diffuseTexture = engineObjectManager->CreateTexture(filename.c_str(),texAttributes);
		}
		else printf("no texture!\n");

		Shader * loadedShader = engineObjectManager->GetLoadedShader(shaderProperties);
		if(loadedShader != NULL)
		{
			MaterialImportDescriptor * textureUVMap = new MaterialImportDescriptor();
			Material * newMaterial = engineObjectManager->CreateMaterial("_Default",loadedShader);
			if(diffuseTexture != NULL)
			{
				std::string diffuseTextureName = GetBuiltinVariableNameForShaderMaterialCharacteristic(ShaderMaterialCharacteristic::DiffuseTextured);
				if(!diffuseTextureName.empty())newMaterial->SetTexture(diffuseTexture, diffuseTextureName);

				int mappedIndex;
				if(AI_SUCCESS==aiGetMaterialInteger(material,AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE,0),&mappedIndex))
				{
					textureUVMap->uvMapping[ShaderMaterialCharacteristic::DiffuseTextured] = mappedIndex;
				}
				else textureUVMap->uvMapping[ShaderMaterialCharacteristic::DiffuseTextured] = 0;
			}
			materials.push_back(newMaterial);
			materialImportDescriptors.push_back(textureUVMap);
		}
		else
		{
			std::string msg = "Could not find loaded shader for: ";
			msg += std::bitset< 64 >(shaderProperties).to_string();
			Debug::PrintError(msg);
			return false;
		}
	}

	return true;
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
