#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scenemanager.h"
#include "engine.h"
#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "geometry/sceneobjecttransform.h"
#include "object/sceneobject.h"
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
	 * Kick off scene processing from the scene root.
	 */
	void SceneManager::Update()
	{
		SceneObjectSharedPtr sceneRoot = (SceneObjectSharedPtr)Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
		ASSERT(sceneRoot.IsValid(), "SceneManager::Update -> 'sceneRoot' is null.");

		Transform baseTransform;
		ProcessScene(sceneRoot.GetRef(), baseTransform);
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
			SceneObjectSharedPtr child = parent.GetChildAt(i);

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
				aggregateTransform.TransformBy(localTransform);

				// save the aggregate/global/world transform
				child->SetAggregateTransform(aggregateTransform);

				// process the child object
				ProcessSceneObject(child.GetRef());

				// continue recursion through child object
				ProcessScene(child.GetRef(), aggregateTransform);

				// restore previous view transform
				PopTransformData(aggregateTransform, sceneProcessingStack);
			}
		}
	}

	/*
	* Process a single scene object.
	*/
	void SceneManager::ProcessSceneObject(SceneObject& object)
	{

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
