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

void SceneObjectTransform::Translate(float x, float y, float z, bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		full.SetTo(this);

		SceneObject * parent = sceneObject->GetParent();
		while(parent != NULL)
		{
			full.PreTransformBy(parent->GetTransform());
			parent = parent->GetParent();
		}
		full.Invert();
		full.GetMatrix()->Transform(trans);

		matrix.PreTranslate(trans[0], trans[1], trans[2]);
	}
	else matrix.Translate(x,y,z);
}
