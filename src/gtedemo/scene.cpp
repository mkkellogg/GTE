//forward declarations
class AssetImporter;
class Vector3;
class Quaternion;
class LavaField;

#include "scene.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/intmask.h"
#include <functional>
#include <vector>


Scene::Scene()
{
	baseCameraForward = Vector3::Forward;
}

Scene::~Scene()
{

}

