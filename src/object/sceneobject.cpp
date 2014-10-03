#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <memory>

#include "sceneobject.h"
#include "engineobject.h"
#include "enginetypes.h"
#include "engineobjectmanager.h"
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

bool SceneObject::SetMeshRenderer3D(Mesh3DRendererRef renderer)
{
	if(this->renderer3D == renderer)return true;
	SHARED_REF_CHECK(renderer,"SceneObject::SetMeshRenderer3D -> attempted to add NULL renderer.", false);

	SceneObjectRef thisRef = EngineObjectManager::Instance()->FindSceneObjectInDirectory(GetObjectID());
	SHARED_REF_CHECK(thisRef,"SceneObject::SetMeshRenderer3D -> Could not find matching reference for scene object", false);

	renderer->sceneObject = thisRef;
	this->renderer3D = renderer;
	if(this->mesh3D.IsValid())
	{
		this->renderer3D->UpdateFromMeshes();
	}

	return true;
}


bool SceneObject::SetMesh3D(Mesh3DRef mesh)
{
	if(this->mesh3D == mesh)return true;
	SHARED_REF_CHECK(mesh,"SceneObject::SetMesh3D -> attempted to add NULL mesh.", false);

	SceneObjectRef thisRef = EngineObjectManager::Instance()->FindSceneObjectInDirectory(GetObjectID());
	SHARED_REF_CHECK(thisRef,"SceneObject::SetMesh3D -> Could not find matching reference for scene object", false);

	mesh->sceneObject = thisRef;
	this->mesh3D = mesh;
	if(this->renderer3D.IsValid())
	{
		this->renderer3D->UpdateFromMeshes();
	}


	return true;
}

bool SceneObject::SetCamera(CameraRef camera)
{
	SHARED_REF_CHECK(camera,"SceneObject::SetCamera -> attempted to add NULL camera.", false);

	SceneObjectRef thisRef = EngineObjectManager::Instance()->FindSceneObjectInDirectory(GetObjectID());
	SHARED_REF_CHECK(thisRef,"SceneObject::SetCamera -> Could not find matching reference for scene object", false);

	camera->sceneObject = EngineObjectManager::Instance()->FindSceneObjectInDirectory(GetObjectID());
	this->camera = camera;

	return true;
}

bool SceneObject::SetLight(LightRef light)
{
	SHARED_REF_CHECK(light,"SceneObject::SetLight -> attempted to add NULL light.", false);

	SceneObjectRef thisRef = EngineObjectManager::Instance()->FindSceneObjectInDirectory(GetObjectID());
	SHARED_REF_CHECK(thisRef,"SceneObject::SetLight -> Could not find matching reference for scene object", false);

	light->sceneObject = thisRef;
	this->light = light;

	return true;
}

Mesh3DRef SceneObject::GetMesh3D()
{
	return mesh3D;
}

Mesh3DRendererRef SceneObject::GetRenderer3D()
{
	return renderer3D;
}

CameraRef SceneObject::GetCamera()
{
	return camera;
}

LightRef SceneObject::GetLight()
{
	return light;
}

void SceneObject::AddChild(SceneObjectRef child)
{
	SHARED_REF_CHECK_RTRN(child,"SceneObject::AddChild -> child is NULL.");

	// check for adding self as child (that's bad)
	if(child.GetPtr() == this)
	{
		Debug::PrintError("SceneObject::AddChild -> Attempted to add self as a child scene object.");
		return;
	}

	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	SceneObjectRef sceneObjectRef = objectManager->FindSceneObjectInDirectory(this->GetObjectID());
	SHARED_REF_CHECK_RTRN(sceneObjectRef,"SceneObject::AddChild -> sceneObjectRef is NULL.");

	//TODO: add check for duplicate children
	if(child->parent.IsValid())
	{
		child->parent->RemoveChild(child);
	}

	child->parent = sceneObjectRef;
	children.push_back(child);
}

void SceneObject::RemoveChild(SceneObjectRef child)
{
	SHARED_REF_CHECK_RTRN(child,"SceneObject::RemoveChild -> child is NULL.");

	int foundIndex = -1;
	for(unsigned int i =0; i < children.size(); i++)
	{
		if (children[i] == child)
		{
			foundIndex = (int)i;
			break;
		}
	}

	if(foundIndex >=0)
	{
		children.erase(children.begin() + foundIndex);
		child->parent = SceneObjectRef::Null();
	}
}

unsigned int SceneObject::GetChildrenCount() const
{
	return children.size();
}

SceneObjectRef SceneObject::GetChildAt(unsigned int index) const
{
	if(index < children.size())
	{
		return children[index];
	}
	else
	{
		Debug::PrintError("SceneObject::GetChildAt -> index out of range.");
		return SceneObjectRef::Null();
	}
}

SceneObjectRef SceneObject::GetParent()
{
	return parent;
}
