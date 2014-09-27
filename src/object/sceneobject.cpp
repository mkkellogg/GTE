#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "sceneobject.h"
#include "engineobject.h"
#include "graphics/graphics.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/render/rendermanager.h"
#include "ui/debug.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"

SceneObject::SceneObject() : EngineObject()
{
	isActive = true;
	subrenderer3D = NULL;
	submesh3D = NULL;
	renderer3D = NULL;
	mesh3D = NULL;
	camera = NULL;
	parent = NULL;
	light = NULL;

	transform = new SceneObjectTransform(this);
}

SceneObject::~SceneObject()
{
	SAFE_DELETE(transform);
}

bool SceneObject::IsActive()
{
	return isActive;
}

void SceneObject::SetActive(bool active)
{
	this->isActive = active;
}

Transform * SceneObject::GetLocalTransform() const
{
	return transform;
}

void SceneObject::GetFullTransform(Transform * transform)
{
	NULL_CHECK_RTRN(transform,"SceneObject::GetFullTransform -> transform is NULL.");

	this->transform->GetFullTransform(transform);
}

bool SceneObject::SetMeshRenderer3D(Mesh3DRenderer * renderer)
{
	if(renderer3D == renderer)return true;
	if(renderer != NULL)
	{
		renderer->sceneObject = this;
		renderer3D = renderer;
		renderer3D->UpdateFromMeshes();
	}
	else
	{
		Debug::PrintError("SceneObject::SetMeshRenderer3D -> attempted to add NULL renderer.");
	}
	return true;
}

bool SceneObject::SetSubMeshRenderer3D(SubMesh3DRenderer *renderer)
{
	if(subrenderer3D == renderer)return true;
	if(renderer != NULL)
	{
		renderer->sceneObject = this;
		subrenderer3D = renderer;
		subrenderer3D->UpdateMeshData();
	}
	else
	{
		Debug::PrintError("SceneObject::SetSubMeshRenderer3D -> attempted to add NULL renderer.");
	}
	return true;
}

bool SceneObject::SetMesh3D(Mesh3D *mesh)
{
	if(mesh != NULL)
	{
		mesh->sceneObject = this;
		this->mesh3D = mesh;
		if(renderer3D !=NULL)
		{
			//renderer3D->UpdateMesh();
		}
	}
	else
	{
		Debug::PrintError("SceneObject::SetMesh3D -> attempted to add NULL mesh.");
	}

	return true;
}

bool SceneObject::SetSubMesh3D(SubMesh3D *mesh)
{
	if(mesh != NULL)
	{
		mesh->sceneObject = this;
		this->submesh3D = mesh;
		if(subrenderer3D !=NULL)
		{
			subrenderer3D->UpdateMeshData();
		}
	}
	else
	{
		Debug::PrintError("SceneObject::SetSubMesh3D -> attempted to add NULL mesh.");
	}
	return true;
}

bool SceneObject::SetCamera(Camera * camera)
{
	this->camera = camera;
	camera->sceneObject = this;
	return true;
}

bool SceneObject::SetLight(Light * light)
{
	this->light = light;
	light->sceneObject = this;
	return true;
}

SubMesh3D * SceneObject::GetSubMesh3D()
{
	return submesh3D;
}

SubMesh3DRenderer * SceneObject::GetSubRenderer3D()
{
	return subrenderer3D;
}

Mesh3D * SceneObject::GetMesh3D()
{
	return mesh3D;
}

Mesh3DRenderer * SceneObject::GetRenderer3D()
{
	return renderer3D;
}

Camera * SceneObject::GetCamera()
{
	return camera;
}

Light *  SceneObject::GetLight()
{
	return light;
}

void SceneObject::AddChild(SceneObject * child)
{
	//TODO: add check for duplicate children

	if(child->parent != NULL)
	{
		child->parent->RemoveChild(child);
	}
	child->parent = this;

	children.push_back(child);
}

void SceneObject::RemoveChild(SceneObject * child)
{
	NULL_CHECK_RTRN(child,"SceneObject::RemoveChild -> adding NULL child.");

	int foundIndex = -1;
	for(unsigned int i =0; i < children.size(); i++)
	{
		if (children[i] == child)
		{
			foundIndex = (int)i;
			break;
		}
	}

	if(foundIndex >=0)children.erase(children.begin() + foundIndex);

	child->parent = NULL;
}

unsigned int SceneObject::GetChildrenCount() const
{
	return children.size();
}

SceneObject * SceneObject::GetChildAt(unsigned int index) const
{
	if(index < children.size())
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
