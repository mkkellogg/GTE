#include <iostream>
#include <math.h>
#include <memory.h>
#include "sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "transform.h"
#include "matrix4x4.h"
#include "ui/debug.h"
#include "global/constants.h"
#include "global/global.h"
#include "vector/vector3.h"
#include "point/point3.h"
#include "util/datastack.h"


SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject) : Transform()
{
	this->sceneObject = sceneObject;
}

SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject, Matrix4x4 * m) : Transform(m)
{
	this->sceneObject = sceneObject;
}

SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject, SceneObjectTransform * sceneObjectTransform) :  Transform(sceneObjectTransform)
{
	this->sceneObject = sceneObject;
}

SceneObjectTransform::~SceneObjectTransform()
{

}

void SceneObjectTransform::GetInheritedTransform(Transform * transform, bool invert)
{
	Transform full;
	SceneObject * parent = sceneObject->GetParent();
	while(parent != NULL)
	{
		full.PreTransformBy(parent->GetTransform());
		parent = parent->GetParent();
	}
	if(invert == true)full.Invert();
	transform->SetTo(&full);
}

void SceneObjectTransform::Translate(float x, float y, float z, bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		GetInheritedTransform(&full, false);
		full.TransformBy(this);
		full.Invert();

		full.GetMatrix()->Transform(trans);

		matrix.Translate(trans[0], trans[1], trans[2]);
	}
	else matrix.Translate(x,y,z);
}

void SceneObjectTransform::RotateAround(float px, float py, float pz, float ax, float ay, float az, float angle)
{
	/*matrix.PreTranslate(-px,-py,-pz);
	matrix.PreRotate(ax,ay,az,angle);
	matrix.PreTranslate(px,py,pz);*/

	float pointTrans[] = {px,py,pz,1};
	float rotVector[] = {ax,ay,az,0};

	Transform mod;
	GetInheritedTransform(&mod, false);
	mod.TransformBy(this);

	mod.Invert();
	mod.GetMatrix()->Transform(pointTrans);
	mod.GetMatrix()->Transform(rotVector);

	float diffX = pointTrans[0];
	float diffY = pointTrans[1];
	float diffZ = pointTrans[2];

	matrix.Translate(diffX,diffY,diffZ);
	matrix.Rotate(rotVector[0],rotVector[1],rotVector[2],angle);
	matrix.Translate(-diffX,-diffY,-diffZ);
}
