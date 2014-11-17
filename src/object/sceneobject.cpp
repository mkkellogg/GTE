#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <memory>

#include "sceneobject.h"
#include "engine.h"
#include "engineobject.h"
#include "enginetypes.h"
#include "engineobjectmanager.h"
#include "graphics/graphics.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/render/rendermanager.h"
#include "ui/debug.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"

SceneObject::SceneObject() : EngineObject()
{
	isActive = true;

	transform.SetIdentity();
	processingTransform.SetIdentity();
}

SceneObject::~SceneObject()
{

}

bool SceneObject::IsActive()
{
	return isActive;
}

void SceneObject::SetActive(bool active)
{
	this->isActive = active;
}

void SceneObject::SetName(const std::string& name)
{
	this->name = name;
}

const std::string& SceneObject::GetName()
{
	return name;
}

Transform& SceneObject::GetLocalTransform()
{
	return transform;
}

/*
 * Attach the SceneObjectTransform [transform] to this scene object.
 */
void SceneObject::InitSceneObjectTransform(SceneObjectTransform * transform)
{
	ASSERT_RTRN(transform != NULL,"SceneObject::GetFullTransform -> transform is NULL.");

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT_RTRN(thisRef.IsValid(),"SceneObject::InitSceneObjectTransform -> Could not find matching reference for scene object");

	transform->AttachTo(thisRef);
	transform->SetLocalTransform(&this->transform);
}

void SceneObject::SetProcessingTransform(Transform * transform)
{
	ASSERT_RTRN(transform != NULL,"SceneObject::SetAggregateTransform -> transform is NULL.");
	processingTransform.SetTo(transform);
}

const Transform& SceneObject::GetProcessingTransform() const
{
	return processingTransform;
}

bool SceneObject::SetMesh3DRenderer(Mesh3DRendererRef renderer)
{
	if(this->renderer3D == renderer)return true;
	ASSERT(renderer.IsValid(),"SceneObject::SetMeshRenderer3D -> attempted to add NULL renderer.", false);

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT(thisRef.IsValid(),"SceneObject::SetMeshRenderer3D -> Could not find matching reference for scene object", false);

	renderer->sceneObject = thisRef;
	this->renderer3D = renderer;
	if(this->mesh3D.IsValid())
	{
		this->renderer3D->UpdateFromMesh();
	}

	return true;
}

bool SceneObject::SetSkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer)
{
	if(this->skinnedRenderer3D == renderer)return true;
	ASSERT(renderer.IsValid(),"SceneObject::SkinnedMesh3DRendererRef -> attempted to add NULL renderer.", false);

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT(thisRef.IsValid(),"SceneObject::SkinnedMesh3DRendererRef -> Could not find matching reference for scene object", false);

	renderer->sceneObject = thisRef;
	this->skinnedRenderer3D = renderer;

	this->skinnedRenderer3D->UpdateFromMesh();

	return true;
}

bool SceneObject::SetMesh3D(Mesh3DRef mesh)
{
	if(this->mesh3D == mesh)return true;
	ASSERT(mesh.IsValid(),"SceneObject::SetMesh3D -> attempted to add NULL mesh.", false);

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT(thisRef.IsValid(),"SceneObject::SetMesh3D -> Could not find matching reference for scene object", false);

	mesh->sceneObject = thisRef;
	this->mesh3D = mesh;
	if(this->renderer3D.IsValid())
	{
		this->renderer3D->UpdateFromMesh();
	}


	return true;
}

bool SceneObject::SetCamera(CameraRef camera)
{
	ASSERT(camera.IsValid(),"SceneObject::SetCamera -> attempted to add NULL camera.", false);

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT(thisRef.IsValid(),"SceneObject::SetCamera -> Could not find matching reference for scene object", false);

	camera->sceneObject = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	this->camera = camera;

	return true;
}

bool SceneObject::SetLight(LightRef light)
{
	ASSERT(light.IsValid(),"SceneObject::SetLight -> attempted to add NULL light.", false);

	SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
	ASSERT(thisRef.IsValid(),"SceneObject::SetLight -> Could not find matching reference for scene object", false);

	light->sceneObject = thisRef;
	this->light = light;

	return true;
}

Mesh3DRef SceneObject::GetMesh3D()
{
	return mesh3D;
}

Mesh3DRendererRef SceneObject::GetMesh3DRenderer()
{
	return renderer3D;
}

SkinnedMesh3DRendererRef SceneObject::GetSkinnedMesh3DRenderer()
{
	return skinnedRenderer3D;
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
	ASSERT_RTRN(child.IsValid(),"SceneObject::AddChild -> child is NULL.");

	// check for adding self as child (that's bad)
	if(child.GetPtr() == this)
	{
		Debug::PrintError("SceneObject::AddChild -> Attempted to add self as a child scene object.");
		return;
	}

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	SceneObjectRef sceneObjectRef = objectManager->FindSceneObjectInDirectory(this->GetObjectID());
	ASSERT_RTRN(sceneObjectRef.IsValid(),"SceneObject::AddChild -> sceneObjectRef is NULL.");

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
	ASSERT_RTRN(child.IsValid(),"SceneObject::RemoveChild -> child is NULL.");

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
