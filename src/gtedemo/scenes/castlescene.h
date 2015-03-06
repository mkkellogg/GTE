#ifndef _GTE_CASTLESCENE_H_
#define _GTE_CASTLESCENE_H_

//forward declarations
class AssetImporter;
class Vector3;
class Quaternion;
class LavaField;

#include "gtedemo/scene.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

class CastleScene : public Scene
{
	SceneObjectRef sceneRoot;
	std::vector<SceneObjectRef> pointLights;

	public:

	CastleScene();
	~CastleScene();

	SceneObjectRef GetSceneRoot();
	void Update();
	void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directLightObject, SceneObjectRef playerObject);

	void SetupTerrain(AssetImporter& importer);
	void SetupStructures(AssetImporter& importer);
	void SetupPlants(AssetImporter& importer);
	void SetupExtra(AssetImporter& importer);
	void SetupLights(AssetImporter& importer, SceneObjectRef playerObject);

	std::vector<SceneObjectRef>& GetPointLights();
};

#endif
