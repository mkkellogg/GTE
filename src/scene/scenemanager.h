/*********************************************
*
* class: SceneManager
*
* author: Mark Kellogg
*
* This class manages all scene objects including culling and 
* dispatching of lifecycle events.
*
***********************************************/

#ifndef _GTE_SCENE_MANAGER_H_
#define _GTE_SCENE_MANAGER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "global/constants.h"
#include "util/datastack.h"
#include "geometry/transform.h"

namespace GTE
{
	// forward declaration
	class SceneObject;
	class SceneObjectComponent;
	class Transform;	

	class SceneManager
	{
		enum class UpdatePhase
		{
			Awake = 0,
			Start = 1,
			Update = 2
		};

		// necessary to trigger lifecycle events and manage allocation
		friend class Engine;
		// necessary since SceneObject will need to process instances of SceneObjectComponent
		// as they are added
		friend class SceneObject;

		SceneManager();
		~SceneManager();

		// transform stack used for processing scene hierarchy
		DataStack<Matrix4x4> sceneProcessingStack;

		Int32 maxPhaseReached;
		UInt32 sceneObjectCount;
		SceneObject* sceneObjectList[Constants::MaxSceneObjects];

		void Update(UpdatePhase phase);
		void ProcessScene(SceneObject& parent, Transform& aggregateTransform);
		void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
		void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);
		void ProcessSceneObjectList(UpdatePhase phase);

		void ProcessSceneObjectUpdatePhase(UpdatePhase phase, SceneObject& object);
		void ProcessSceneObjectComponentUpdatePhase(UpdatePhase phase, SceneObjectComponent& component);

		void ProcessSceneObjectAsNew(SceneObject& object);
		void ProcessSceneObjectComponentAsNew(SceneObjectComponent& component);

	public:

		Bool Init();
		void Update();
		void Start();
		void Awake();

	};
}

#endif
