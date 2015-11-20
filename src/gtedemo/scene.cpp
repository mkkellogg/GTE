#include "engine.h"
#include "scene.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/intmask.h"

#include <functional>
#include <vector>

Scene::Scene()
{
	baseCameraForward = GTE::Vector3::Forward;
}

Scene::~Scene()
{

}

