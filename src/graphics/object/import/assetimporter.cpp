#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <fstream>
#include <string>
#include <vector>
#include <map>

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
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "geometry/sceneobjecttransform.h"
#include "graphics/render/material.h"
#include "graphics/image/rawimage.h"

#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"

#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"

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

	scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality);
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
	ProcessMaterials(modelPath, scene, materials);

	SceneObject * root = objectManager->CreateSceneObject();
	NULL_CHECK(root,"AssetImporter::ProcessModelScene -> could not create root object", NULL);

	root->SetActive(false);
	Matrix4x4 baseTransform;
	RecursiveProcessModelScene(scene, scene->mRootNode, importScale, root, &baseTransform);

	return root;
}

void AssetImporter::RecursiveProcessModelScene(const aiScene *scene, const aiNode* nd, float scale, SceneObject * current, Matrix4x4 * currentTransform)
{
	unsigned int i;
	unsigned int n=0, t;
	Matrix4x4 mat;

	aiMatrix4x4 m = nd->mTransformation;
	aiMatrix4x4 m2;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);

	m = m * m2;
	ImportUtil::ConvertAssimpMatrix(&m,&mat);

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();
	Graphics * graphics = Graphics::Instance();

	for (n=0; n < nd->mNumMeshes; n++)
	{
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		Mesh3D * mesh3D = ConvertAssimpMesh(mesh);
		NULL_CHECK_RTRN(mesh3D,"AssetImporter::RecursiveProcessModelScene -> Could not convert Assimp mesh.");

		mesh3D->CalculateNormals(90);

		SceneObject * sceneObject = engineObjectManager->CreateSceneObject();
		NULL_CHECK_RTRN(sceneObject,"AssetImporter::RecursiveProcessModelScene -> Could not create scene object.");

		Mesh3DRenderer * meshRenderer = engineObjectManager->CreateMesh3DRenderer();
		NULL_CHECK_RTRN(meshRenderer,"AssetImporter::RecursiveProcessModelScene -> Could not create mesh renderer.");

		Shader * defaultShader = engineObjectManager->GetBuiltinShader(BuiltinShader::Diffuse);
		Material * material =engineObjectManager->CreateMaterial("_Default", defaultShader);
		NULL_CHECK_RTRN(material,"AssetImporter::RecursiveProcessModelScene -> Could not create material.");

		meshRenderer->SetMaterial(material);

		sceneObject->SetMesh(mesh3D);
		sceneObject->SetMeshRenderer(meshRenderer);
		sceneObject->GetTransform()->SetTo(&mat);
		current->AddChild(sceneObject);
	}

	for(int i=0; i <nd->mNumChildren; i++)
	{
		SceneObject * child = engineObjectManager->CreateSceneObject();
		NULL_CHECK_RTRN(child,"AssetImporter::RecursiveProcessModelScene -> Could not create child scene object.");
		current->AddChild(child);

		const aiNode *node = nd->mChildren[i];
		if(node != NULL)RecursiveProcessModelScene(scene, node, scale, child, &mat);
	}
}

Mesh3D * AssetImporter::ConvertAssimpMesh(const aiMesh* mesh)
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

	if(mesh->HasTextureCoords(0))	//HasTextureCoords(texture_coordinates_set)
	{
		//StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UV1);
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

			vertexComponentIndex+=3;
			vertexIndex++;
		}

		//glEnd();
	}

	return mesh3D;
}

bool AssetImporter::ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials)
{
	ILboolean success;

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		/// wrong DevIL version ///
		std::string msg = "AssetImporter::ProcessMaterials -> wrong DevIL version";
		Debug::PrintError(msg);
		return false;
	}

	ilInit(); /// Initialization of DevIL
	if (scene->HasTextures())
	{
		Debug::PrintError("AssetImporter::ProcessMaterials -> Support for meshes with embedded textures is not implemented");
		return false;
	}

	int textureCount = 0;
	std::vector<std::string> texturePaths;
	// getTexture Filenames and Numb of Textures
	for (unsigned int m=0; m<scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		aiString path;	// filename

		// TODO: enable multiple textures per material here!!
		//while (texFound == AI_SUCCESS)
		//{
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			texturePaths.push_back(std::string(path.data));
			textureCount++;
			texIndex++;
		//}
	}

	EngineObjectManager * engineObjectManager =  EngineObjectManager::Instance();

	std::vector<std::string>::iterator itr = texturePaths.begin();
	std::string basepath = GetBasePath(modelPath);
	for (int i=0; i<textureCount; i++)
	{
		//save IL image ID
		std::string filename = *itr; // get filename

		itr++;	// next texture

		// Convert every colour component into unsigned byte.If your image contains
		// alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			// Error occured
			Debug::PrintError("AssetImporter::ProcessMaterials -> Couldn't convert image");
			return false;
		}

		/*
		ilGetInteger(IL_IMAGE_BPP),
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		ilGetInteger(IL_IMAGE_FORMAT),
		ilGetData()
		*/

		TextureAttributes texAttributes;
		texAttributes.FilterMode = TextureFilter::TriLinear;
		texAttributes.MipMapLevel = 4;

		engineObjectManager->CreateTexture(modelPath.c_str(),texAttributes);

		Shader * defaultShader = engineObjectManager->GetBuiltinShader(BuiltinShader::Diffuse);
		Material * material = engineObjectManager->CreateMaterial("Default",defaultShader);

		materials.push_back(material);
	}

	/*
	int numTextures = textureIdMap.size();
	// array with DevIL image IDs
	ILuint* imageIds = NULL;
	imageIds = new ILuint[numTextures];
	// generate DevIL Image IDs
	ilGenImages(numTextures, imageIds); //Generation of numTextures image names
	// create and fill array with GL texture ids
	textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds); // Texture name generation
	// get iterator
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();
	std::string basepath = getBasePath(modelpath);
	for (int i=0; i<numTextures; i++)
	{
	//save IL image ID
	std::string filename = (*itr).first; // get filename
	(*itr).second = &textureIds[i];	// save texture id for filename in map
	itr++;	// next texture
	ilBindImage(imageIds[i]); // Binding of DevIL image name
	std::string fileloc = basepath + filename;	// Loading of image
	success = ilLoadImage(fileloc.c_str());
	if (success) // If no error occured:
	{
	// Convert every colour component into unsigned byte.If your image contains
	// alpha channel you can replace IL_RGB with IL_RGBA
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	if (!success)
	{
	// Error occured
	abortGLInit("Couldn't convert image");
	return -1;
	}
	// Binding of texture name
	glBindTexture(GL_TEXTURE_2D, textureIds[i]);
	// redefine standard texture values
	// We will use linear interpolation for magnification filter
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// We will use linear interpolation for minifying filter
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// Texture specification
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
	ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
	ilGetData());
	// we also want to be able to deal with odd texture dimensions
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
	}
	else
	{
	// Error occured
	MessageBox(NULL, ("Couldn't load Image: " + fileloc).c_str() , "ERROR", MB_OK | MB_ICONEXCLAMATION);
	}
	}
	// Because we have already copied image data into texture data we can release memory used by image.
	ilDeleteImages(numTextures, imageIds);
	// Cleanup
	delete [] imageIds;
	imageIds = NULL;
	return TRUE;
	}*/

	return true;
}

std::string AssetImporter::GetBasePath(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}
