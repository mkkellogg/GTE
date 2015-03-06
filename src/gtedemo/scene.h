#ifndef _GTE_SCENE_H_
#define _GTE_SCENE_H_

//forward declarations
class AssetImporter;
class Vector3;
class Quaternion;
class LavaField;

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

class Scene
{
	public:

	Scene();
	virtual ~Scene();

	virtual SceneObjectRef GetSceneRoot() = 0;
	virtual void Update() = 0;
	virtual void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directLightObject, SceneObjectRef playerObject) = 0;
};

#endif
