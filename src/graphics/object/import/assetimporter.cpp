#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <fstream>
#include <string>
#include <map>

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

#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"

#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"

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
		return NULL;
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ProcessModelScene(scene, importScale);
}

SceneObject * AssetImporter::ProcessModelScene(const aiScene* scene, float importScale)
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	SceneObject * root = objectManager->CreateSceneObject();
	NULL_CHECK(root,"AssetImporter::ProcessModelScene -> could not create root object", NULL);

	Matrix4x4 baseTransform;
	RecursiveProcessModelScene(scene, scene->mRootNode, importScale, root, &baseTransform);

	return NULL;
}

void AssetImporter::RecursiveProcessModelScene(const aiScene *scene, const aiNode* nd, float scale, SceneObject * parent, Matrix4x4 * currentTransform)
{
	unsigned int i;
	unsigned int n=0, t;
	Matrix4x4 mat;

	aiMatrix4x4 m = nd->mTransformation;
	aiMatrix4x4 m2;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);

	m = m * m2;
	ImportUtil::ConvertAssimpMatrix(&m,&mat);

	for (n=0; n < nd->mNumMeshes; n++)
	{
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
	}
}

Mesh3D * AssetImporter::ConvertAssimpMesh(const aiMesh* mesh)
{
	int faceCount = 0;
	int vertexCount = 0;
	for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
	{
		const aiFace* face = &mesh->mFaces[faceIndex];
		vertexCount += face->mNumIndices;
		faceCount++;
	}

	bool hasNormals = false;

	int vertexComponentIndex = 0;
	int vertexIndex = 0;
	for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
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

		for(int i = face->mNumIndices; i >=0; i--)	// go through all vertices in face
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

			vertexComponentIndex+=3;
			vertexIndex++;
		}

		//glEnd();
	}
	return NULL;
}
