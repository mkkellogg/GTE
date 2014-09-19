#ifndef _ASSET_IMPORTER_H_
#define _ASSET_IMPORTER_H_

//forward declarations
class Matrix4x4;
class EngineObjectManager;
class SceneObject;
class Mesh3D;
class Material;

#include "object/sceneobjectcomponent.h"
#include "graphics/stdattributes.h"
#include "assimp/scene.h"
#include <string>
#include <vector>

class AssetImporter
{
	friend EngineObjectManager;

	protected :

	void RecursiveProcessModelScene(const aiScene *sc, const aiNode* nd, float scale, SceneObject * parent, Matrix4x4 * currentTransform, std::vector<Material *>& materials);
	SceneObject * ProcessModelScene(const std::string& modelPath, const aiScene* scene, float importScale);
	bool ProcessMaterials(const std::string& modelPath, const aiScene *scene, std::vector<Material *>& materials);

	public:

	AssetImporter();
	~AssetImporter();

	SceneObject * LoadModel(const std::string& filePath, float importScale);
	Mesh3D * ConvertAssimpMesh(const aiMesh* mesh);

};

#endif
