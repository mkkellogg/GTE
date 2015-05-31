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
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/render/rendermanager.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"

namespace GTE
{
	SceneObject::SceneObject() : EngineObject()
	{
		isActive = true;
		isStatic = false;

		transform.SetIdentity();
		transform.SetSceneObject(this);

		processingTransform.SetIdentity();

		layerMask = IntMaskUtil::CreateIntMask();
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

	bool SceneObject::IsStatic()
	{
		return isStatic;
	}

	void SceneObject::SetStatic(bool isStatic)
	{
		this->isStatic = isStatic;
	}

	void SceneObject::SetName(const std::string& name)
	{
		this->name = name;
	}

	const std::string& SceneObject::GetName()
	{
		return name;
	}

	void SceneObject::SetLayerMask(IntMask mask)
	{
		layerMask = mask;
	}

	IntMask SceneObject::GetLayerMask() const
	{
		return layerMask;
	}

	SceneObjectTransform& SceneObject::GetTransform()
	{
		return transform;
	}

	void SceneObject::SetAggregateTransform(Transform& transform)
	{
		processingTransform.SetTo(transform);
	}

	const Transform& SceneObject::GetAggregateTransform() const
	{
		return processingTransform;
	}

	void SceneObject::NotifyNewMesh3D()
	{
		Mesh3DRef mesh = GetMesh3D();
		Mesh3DRendererRef mesh3DRenderer = GetMesh3DRenderer();
		if (mesh3DRenderer.IsValid() && mesh.IsValid())mesh3DRenderer->InitializeForMesh();

		SkinnedMesh3DRendererRef skinnedMesh3DRenderer = GetSkinnedMesh3DRenderer();
		if (skinnedMesh3DRenderer.IsValid() && mesh.IsValid())skinnedMesh3DRenderer->InitializeForMesh();
	}

	bool SceneObject::SetMesh3DRenderer(Mesh3DRendererRef renderer)
	{
		if (this->renderer3D == renderer)return true;
		NONFATAL_ASSERT_RTRN(renderer.IsValid(), "SceneObject::SetMeshRenderer3D -> 'renderer' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetMeshRenderer3D -> Could not find matching reference for scene object");

		renderer->sceneObject = thisRef;
		this->renderer3D = renderer;

		NotifyNewMesh3D();

		return true;
	}

	bool SceneObject::SetSkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer)
	{
		if (this->skinnedRenderer3D == renderer)return true;
		NONFATAL_ASSERT_RTRN(renderer.IsValid(), "SceneObject::SkinnedMesh3DRendererRef -> 'renderer' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SkinnedMesh3DRendererRef -> Could not find matching reference for scene object.");

		renderer->sceneObject = thisRef;
		this->skinnedRenderer3D = renderer;

		NotifyNewMesh3D();

		return true;
	}

	bool SceneObject::SetMesh3DFilter(Mesh3DFilterRef filter)
	{
		NONFATAL_ASSERT_RTRN(filter.IsValid(), "SceneObject::SetMesh3DFilter -> 'filter' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetMesh3DFilter -> Could not find matching reference for scene object.");

		filter->sceneObject = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		this->mesh3DFilter = filter;

		NotifyNewMesh3D();

		return true;
	}

	bool SceneObject::SetCamera(CameraRef camera)
	{
		NONFATAL_ASSERT_RTRN(camera.IsValid(), "SceneObject::SetCamera -> 'camera' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetCamera -> Could not find matching reference for scene object.");

		camera->sceneObject = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		this->camera = camera;

		return true;
	}

	bool SceneObject::SetLight(LightRef light)
	{
		NONFATAL_ASSERT_RTRN(light.IsValid(), "SceneObject::SetLight -> 'light' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetLight -> Could not find matching reference for scene object.");

		light->sceneObject = thisRef;
		this->light = light;

		return true;
	}

	Mesh3DRef SceneObject::GetMesh3D()
	{
		if (!mesh3DFilter.IsValid())return Mesh3DRef::Null();
		return mesh3DFilter->GetMesh3D();
	}

	Mesh3DFilterRef SceneObject::GetMesh3DFilter()
	{
		return mesh3DFilter;
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
		NONFATAL_ASSERT(child.IsValid(), "SceneObject::AddChild -> 'child' is invalid.", true);

		// check for adding self as child (that's bad)
		if (child.GetPtr() == this)
		{
			Debug::PrintError("SceneObject::AddChild -> Attempted to add self as a child scene object.");
			return;
		}

		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		SceneObjectRef sceneObjectRef = objectManager->FindSceneObjectInDirectory(this->GetObjectID());
		ASSERT(sceneObjectRef.IsValid(), "SceneObject::AddChild -> Could not find matching reference for 'child' scene object.");

		//TODO: add check for duplicate children
		if (child->parent.IsValid())
		{
			child->parent->RemoveChild(child);
		}

		// by attaching a parent to [child], we are modifying its final transformation
		// into the scene because the transform of its new parent will be concatenated with
		// its own. we want the resulting full word-space transformation of the child
		// to be the same as it was before attaching it to its new parent, so we adjust
		// the local transform of [child] accordingly.
		Transform parentInverse;
		SceneObjectTransform::GetWorldTransform(parentInverse, this, true, true);
		Transform newChildTransform;
		newChildTransform.SetTo(parentInverse);
		newChildTransform.TransformBy(child->GetTransform());
		child->GetTransform().SetTo(newChildTransform);

		child->parent = sceneObjectRef;
		children.push_back(child);
	}

	void SceneObject::RemoveChild(SceneObjectRef child)
	{
		NONFATAL_ASSERT(child.IsValid(), "SceneObject::RemoveChild -> 'child' is invalid.", true);

		Int32 foundIndex = -1;
		for (UInt32 i = 0; i < children.size(); i++)
		{
			if (children[i] == child)
			{
				foundIndex = (Int32)i;
				break;
			}
		}

		if (foundIndex >= 0)
		{
			children.erase(children.begin() + foundIndex);

			// by removing the parent of[child], we are modifying its final transformation
			// into the scene because the transform of its parent was concatenated with
			// its own to form the final transformation. we want the resulting full word-space
			// transformation of the child to be the same as it was before removing it from its
			// parent, so we adjust the local transform of [child] accordingly.
			Transform newChildTransform;
			SceneObjectTransform::GetWorldTransform(newChildTransform, child, true, false);
			child->GetTransform().SetTo(newChildTransform);
			child->parent = SceneObjectRef::Null();
		}
	}

	UInt32 SceneObject::GetChildrenCount() const
	{
		return children.size();
	}

	SceneObjectRef SceneObject::GetChildAt(UInt32 index) const
	{
		if (index < children.size())
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
}
