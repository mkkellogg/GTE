#include "sceneobject.h"
#include "object/engineobject.h"
#include "object/engineobjectmanager.h"
#include "scenemanager.h"
#include "graphics/graphics.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/particles/particlesystem.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"

namespace GTE
{
	SceneObject::SceneObject() : EngineObject()
	{
		isActive = true;
		isStatic = false;
		inheritTransform = true;

		transform.SetIdentity();
		transform.SetSceneObject(this);

		layerMask = IntMaskUtil::CreateIntMask();
	}

	SceneObject::~SceneObject()
	{

	}

	Bool SceneObject::IsActive()
	{
		return isActive;
	}

	void SceneObject::SetActive(Bool active)
	{
		this->isActive = active;
		if(active)
		{
			Engine::Instance()->GetSceneManager()->ProcessSceneObjectAsNew(*this);
		}
	}

	Bool SceneObject::IsStatic()
	{
		return isStatic;
	}

	void SceneObject::SetStatic(Bool isStatic)
	{
		this->isStatic = isStatic;
	}

	Bool SceneObject::InheritsTransform() const
	{
		return inheritTransform;
	}

	void SceneObject::SetInheritsTransform(Bool inherit)
	{
		this->inheritTransform = inheritTransform;
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

	const SceneObjectTransform& SceneObject::GetConstTransform() const
	{
		return transform;
	}

	SceneObjectProcessingDescriptor& SceneObject::GetProcessingDescriptor()
	{
		return processingDescriptor;
	}

	void SceneObject::NotifyNewMesh3D()
	{
		Mesh3DSharedPtr mesh = GetMesh3D();

		Mesh3DRendererSharedPtr mesh3DRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(renderer);
		SkinnedMesh3DRendererSharedPtr skinnedMesh3DRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::SkinnedMesh3DRenderer>(renderer);

		if (mesh3DRenderer.IsValid() && mesh.IsValid())mesh3DRenderer->InitializeForMesh();
		if (skinnedMesh3DRenderer.IsValid() && mesh.IsValid())skinnedMesh3DRenderer->InitializeForMesh();
	}

	Bool SceneObject::SetRenderer(RendererRef renderer)
	{
		if(this->renderer == renderer)return true;
		NONFATAL_ASSERT_RTRN(renderer.IsValid(), "SceneObject::SetRenderer -> 'renderer' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetRenderer -> Could not find matching reference for scene object");

		renderer->sceneObject = thisRef;
		this->renderer = renderer;

		NotifyNewMesh3D();

		Engine::Instance()->GetSceneManager()->ProcessSceneObjectComponentAsNew(renderer.GetRef());

		return true;
	}

	Bool SceneObject::SetMesh3DFilter(Mesh3DFilterRef filter)
	{
		NONFATAL_ASSERT_RTRN(filter.IsValid(), "SceneObject::SetMesh3DFilter -> 'filter' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetMesh3DFilter -> Could not find matching reference for scene object.");

		filter->sceneObject = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		this->mesh3DFilter = filter;

		NotifyNewMesh3D();

		Engine::Instance()->GetSceneManager()->ProcessSceneObjectComponentAsNew(filter.GetRef());

		return true;
	}

	Bool SceneObject::SetCamera(CameraRef camera)
	{
		NONFATAL_ASSERT_RTRN(camera.IsValid(), "SceneObject::SetCamera -> 'camera' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetCamera -> Could not find matching reference for scene object.");

		camera->sceneObject = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		this->camera = camera;

		Engine::Instance()->GetSceneManager()->ProcessSceneObjectComponentAsNew(camera.GetRef());

		return true;
	}

	Bool SceneObject::SetLight(LightRef light)
	{
		NONFATAL_ASSERT_RTRN(light.IsValid(), "SceneObject::SetLight -> 'light' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetLight -> Could not find matching reference for scene object.");

		light->sceneObject = thisRef;
		this->light = light;

		Engine::Instance()->GetSceneManager()->ProcessSceneObjectComponentAsNew(light.GetRef());

		return true;
	}

	Bool SceneObject::SetParticleSystem(ParticleSystemRef particleSystem)
	{
		NONFATAL_ASSERT_RTRN(particleSystem.IsValid(), "SceneObject::SetParticleSystem -> 'particleSystem' is invalid.", false, true);

		SceneObjectRef thisRef = Engine::Instance()->GetEngineObjectManager()->FindSceneObjectInDirectory(GetObjectID());
		ASSERT(thisRef.IsValid(), "SceneObject::SetParticleSystem -> Could not find matching reference for scene object.");

		particleSystem->sceneObject = thisRef;
		this->particleSystem = particleSystem;

		Engine::Instance()->GetSceneManager()->ProcessSceneObjectComponentAsNew(particleSystem.GetRef());

		return true;
	}

	Bool SceneObject::RemoveRenderer()
	{
		NONFATAL_ASSERT_RTRN(renderer.IsValid(), "SceneObject::RemoveRenderer -> Scene object has no renderer.", false, true);

		renderer->sceneObject = SceneObjectSharedPtr::Null();
		this->renderer = RendererSharedPtr::Null();

		return true;
	}

	Bool SceneObject::RemoveMesh3DFilter()
	{
		NONFATAL_ASSERT_RTRN(mesh3DFilter.IsValid(), "SceneObject::RemoveMesh3DFilter -> Scene object has no mesh filter.", false, true);

		mesh3DFilter->sceneObject = SceneObjectSharedPtr::Null();
		this->mesh3DFilter = Mesh3DFilterSharedPtr::Null();

		return true;
	}

	Bool SceneObject::RemoveCamera()
	{
		NONFATAL_ASSERT_RTRN(camera.IsValid(), "SceneObject::RemoveCamera -> Scene object has no camera.", false, true);

		camera->sceneObject = SceneObjectSharedPtr::Null();
		this->camera = CameraSharedPtr::Null();

		return true;
	}

	Bool SceneObject::RemoveLight()
	{
		NONFATAL_ASSERT_RTRN(light.IsValid(), "SceneObject::RemoveLight -> Scene object has no light.", false, true);

		light->sceneObject = SceneObjectSharedPtr::Null();
		this->light = LightSharedPtr::Null();
		return true;

	}

	Bool SceneObject::RemoveParticleSystem()
	{
		NONFATAL_ASSERT_RTRN(particleSystem.IsValid(), "SceneObject::RemoveParticleSystem -> Scene object has no particle system.", false, true);

		particleSystem->sceneObject = SceneObjectSharedPtr::Null();
		this->particleSystem = ParticleSystemSharedPtr::Null();
		return true;

	}

	Mesh3DRef SceneObject::GetMesh3D()
	{
		if (!mesh3DFilter.IsValid())return NullMesh3DRef;
		return mesh3DFilter->GetMesh3D();
	}

	Mesh3DFilterRef SceneObject::GetMesh3DFilter()
	{
		return mesh3DFilter;
	}

	RendererRef SceneObject::GetRenderer()
	{
		return renderer;
	}

	CameraRef SceneObject::GetCamera()
	{
		return camera;
	}

	LightRef SceneObject::GetLight()
	{
		return light;
	}

	ParticleSystemRef SceneObject::GetParticleSystem()
	{
		return particleSystem;
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
			child->parent = SceneObjectSharedPtr::Null();
		}
	}

	UInt32 SceneObject::GetChildrenCount() const
	{
		return (UInt32)children.size();
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
			return NullSceneObjectRef;
		}
	}

	SceneObjectRef SceneObject::GetParent() const
	{
		return parent;
	}
}
