#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "scenemanager.h"
#include "object/engineobject.h"
#include "scene/sceneobject.h"
#include "scene/sceneobjectcomponent.h"
#include "object/engineobjectmanager.h"
#include "scene/eventmanager.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "util/datastack.h"
#include "debug/gtedebug.h"

#include <string>
#include <unordered_map>

namespace GTE
{
	/*
	* Default constructor
	*/
	SceneManager::SceneManager() : sceneProcessingStack(Constants::MaxObjectRecursionDepth, 1)
	{
		sceneObjectCount = 0;
		maxPhaseReached = -1;
	}

	/*
	 * Destructor
	 */
	SceneManager::~SceneManager()
	{

	}

	

	/*
	* Initialize. Return false if initialization false, true if it succeeds.
	*/
	Bool SceneManager::Init()
	{
		ASSERT(sceneProcessingStack.Init(), "SceneManager::Init -> Unable to initialize view transform stack.");
		return true;
	}

	/*
	* Called once when the scene manager first starts.
	*/
	void SceneManager::Awake()
	{
		Update(UpdatePhase::Awake);
		maxPhaseReached = (Int32)UpdatePhase::Awake;
	}

	/*
	* Called once when the scene manager first starts, but after Awake().
	*/
	void SceneManager::Start()
	{
		Update(UpdatePhase::Start);
		maxPhaseReached = (Int32)UpdatePhase::Start;
	}

	/*
	 * Kick off scene processing from the scene root.
	 */
	void SceneManager::Update()
	{
		Update(UpdatePhase::Update);
		maxPhaseReached = (Int32)UpdatePhase::Update;
	}

	/*
	* Kick off scene processing from the scene root.
	*/
	void SceneManager::Update(UpdatePhase phase)
	{
		SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
		ASSERT(sceneRoot.IsValid(), "SceneManager::Update -> 'sceneRoot' is null.");

		sceneObjectCount = 0;

		Transform baseTransform;
		// form list of scene objects
		ProcessScene(sceneRoot.GetRef(), baseTransform);
		// process resulting list of scene objects
		ProcessSceneObjectList(phase);
	}

	/*
	*
	* Recursively visits each object in the scene that is reachable from [parent]. The transforms of each
	* scene object are concatenated as progress moves down the scene object tree and passed to the current
	* invocation via [aggregateTransform]. These aggregate transforms are saved to each SceneObject via
	* SceneObject::SetAggregateTransform().
	*
	*/
	void SceneManager::ProcessScene(SceneObject& parent, Transform& aggregateTransform)
	{
		// enforce max recursion depth
		if(sceneProcessingStack.GetEntryCount() >= Constants::MaxObjectRecursionDepth - 1)return;

		for(UInt32 i = 0; i < parent.GetChildrenCount(); i++)
		{
			SceneObjectRef child = parent.GetChildAt(i);

			if(!child.IsValid())
			{
				Debug::PrintWarning("SceneManager::PreProcessScene -> Null scene object encountered.");
				continue;
			}

			// only process active scene objects
			if(child->IsActive())
			{
				// save the existing view transform
				PushTransformData(aggregateTransform, sceneProcessingStack);

				// concatenate the current view transform with that of the current scene object
				Transform& localTransform = child->GetTransform();
				if(!child->InheritsTransform())
				{
					aggregateTransform.SetIdentity();
				}
				aggregateTransform.TransformBy(localTransform);

				// save the aggregate/global/world transform
				SceneObjectProcessingDescriptor& processingDesc = child->GetProcessingDescriptor();
				processingDesc.AggregateTransform = aggregateTransform;
				processingDesc.AggregateTransformInverse = aggregateTransform;
				processingDesc.AggregateTransformInverse.Invert();

				sceneObjectList[sceneObjectCount] = child.GetPtr();
				sceneObjectCount++;
				if(sceneObjectCount >= Constants::MaxSceneObjects)return;

				// continue recursion through child object
				ProcessScene(child.GetRef(), aggregateTransform);

				// restore previous view transform
				PopTransformData(aggregateTransform, sceneProcessingStack);
			}
		}
	}

	void SceneManager::ProcessSceneObjectList(UpdatePhase phase)
	{
		for(UInt32 i = 0; i < sceneObjectCount; i++)
		{
			SceneObject * sceneObject = sceneObjectList[i];
			
			ProcessSceneObjectUpdatePhase(phase, *sceneObject);
		}
	}

	void SceneManager::ProcessSceneObjectUpdatePhase(UpdatePhase phase, SceneObject& object)
	{
		switch(phase)
		{
			case UpdatePhase::Awake:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Awake, object);
			break;
			case UpdatePhase::Start:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Start, object);
			break;
			case UpdatePhase::Update:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Update, object);
			break;
		}
	}

	void SceneManager::ProcessSceneObjectComponentUpdatePhase(UpdatePhase phase, SceneObjectComponent& component)
	{
		switch(phase)
		{
			case UpdatePhase::Awake:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Awake, component);
			break;
			case UpdatePhase::Start:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Start, component);
			break;
			case UpdatePhase::Update:
				Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Update, component);
			break;
		}
	}

	/*
	* Fully process a SceneObject instance as if it were newly added.
	*/
	void SceneManager::ProcessSceneObjectAsNew(SceneObject& object)
	{
		// only process active scene objects
		if(object.IsActive())
		{
			Transform aggregateTransform;
			SceneObjectTransform::GetWorldTransform(aggregateTransform, object, true, false);

			// save the aggregate/global/world transform
			SceneObjectProcessingDescriptor& processingDesc = object.GetProcessingDescriptor();
			processingDesc.AggregateTransform = aggregateTransform;
			processingDesc.AggregateTransformInverse = aggregateTransform;
			processingDesc.AggregateTransformInverse.Invert();

			if(maxPhaseReached >= (Int32)UpdatePhase::Awake)
			{
				ProcessSceneObjectUpdatePhase(UpdatePhase::Awake, object);
			}

			if(maxPhaseReached >= (Int32)UpdatePhase::Start)
			{
				ProcessSceneObjectUpdatePhase(UpdatePhase::Start, object);
			}
		}
	}

	/*
	* Fully process a SceneObjectComponent instance as if it were newly added.
	*/
	void SceneManager::ProcessSceneObjectComponentAsNew(SceneObjectComponent& component)
	{
		SceneObjectRef container = component.GetSceneObject();
		NONFATAL_ASSERT(container.IsValid(), "SceneManager::ProcessSceneObjectComponentAsNew -> Component's container is not valid!", true);

		// only process components that belong to active scene objects
		if(container->IsActive())
		{
			if(maxPhaseReached >= (Int32)UpdatePhase::Awake)
			{
				ProcessSceneObjectComponentUpdatePhase(UpdatePhase::Awake, component);
			}

			if(maxPhaseReached >= (Int32)UpdatePhase::Start)
			{
				ProcessSceneObjectComponentUpdatePhase(UpdatePhase::Start, component);
			}
		}
	}

	/*
	* Save a transform to the transform stack. This method is used to to save transformations
	* as the render manager progresses through the object tree that makes up the scene.
	*/
	void SceneManager::PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack)
	{
		const Matrix4x4& matrix = transform.GetConstMatrix();
		transformStack.Push(&matrix);
	}

	/*
	* Remove the top transform from the transform stack.
	*/
	void SceneManager::PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack)
	{
		NONFATAL_ASSERT(transformStack.GetEntryCount() > 0, "SceneManager::PopTransformData -> 'transformStack' is empty!", true);

		Matrix4x4 * mat = transformStack.Pop();
		transform.SetTo(*mat);
	}
}
