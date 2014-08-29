#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "sceneobject.h"
#include "engineobject.h"
#include "graphics/graphics.h"
#include "graphics/view/camera.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendermanager.h"
#include "ui/debug.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"

SceneObject::SceneObject() : EngineObject()
{
	renderer3D = NULL;
	mesh3D = NULL;
	camera = NULL;
	parent = NULL;

	transform = new SceneObjectTransform(this);
}

SceneObject::~SceneObject()
{
	SAFE_DELETE(transform);
}

SceneObjectTransform * SceneObject::GetTransform() const
{
	return transform;
}

bool SceneObject::SetMeshRenderer(Mesh3DRenderer *renderer)
{
	if(renderer != NULL && renderer3D != renderer)
	{
		this->renderer3D = renderer;
		if(mesh3D != NULL)
		{
			renderer3D->UseMesh(mesh3D);
		}
	}
	else
	{
		Debug::PrintError("SceneObject::AddMeshRenderer -> attempted to add NULL renderer.");
	}
	return true;
}

bool SceneObject::SetMesh(Mesh3D *mesh)
{
	if(mesh != NULL)
	{
		this->mesh3D = mesh;
		if(renderer3D != NULL)
		{
			renderer3D->UseMesh(mesh);
		}
	}
	else
	{
		Debug::PrintError("SceneObject::AddMesh -> attempted to add NULL mesh.");
	}
	return true;
}

bool SceneObject::SetCamera(Camera * camera)
{
	this->camera = camera;
	return true;
}

Mesh3DRenderer * SceneObject::GetRenderer3D()
{
	return renderer3D;
}

Camera * SceneObject::GetCamera()
{
	return camera;
}

void SceneObject::AddChild(SceneObject * child)
{
	//TODO: add check for duplicate children

	if(child->parent != NULL)
	{
		parent->RemoveChild(child);
	}

	child->parent = this;
	children.push_back(child);
}

void SceneObject::RemoveChild(SceneObject * child)
{
	if(child != NULL)
	{
		for(std::vector<SceneObject *>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if (*it == child)
			{
				children.erase(it);
				break;
			}
		}

		child->parent = NULL;
	}
	else
	{
		Debug::PrintError("SceneObject::RemoveChild -> adding NULL child.");
	}
}

unsigned int SceneObject::GetChildrenCount() const
{
	return children.size();
}

SceneObject * SceneObject::GetChildAt(int index) const
{
	if(index >=0 && index < (int)children.size())
	{
		return children[index];
	}
	else
	{
		Debug::PrintError("SceneObject::GetChildAt -> index out of range.");
		return NULL;
	}
}

SceneObject * SceneObject::GetParent()
{
	return parent;
}
