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

#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "util/datastack.h"
#include "geometry/transform.h"
#include <unordered_map>

namespace GTE
{
	// forward declaration
	class SceneObject;
	class Transform;

	class SceneManager
	{
		friend class Engine;

		SceneManager();
		~SceneManager();

		// map object IDs of Skeleton objects to their assign animation player
		std::unordered_map<ObjectID, AnimationPlayerSharedPtr> activePlayers;

		// transform stack used for processing scene hierarchy
		DataStack<Matrix4x4> sceneProcessingStack;

		void ProcessScene(SceneObject& parent, Transform& aggregateTransform);
		void ProcessSceneObject(SceneObject& object);
		void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
		void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);

	public:

		Bool Init();
		void Update();

	};
}

#endif
