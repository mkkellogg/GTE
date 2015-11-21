/*********************************************
*
* class: AnimationManager
*
* author: Mark Kellogg
*
* This class manages and drives all instances of AnimationPlayer, and
* therefore, all playing animations.
*
***********************************************/

#ifndef _GTE_ANIMATION_MANAGER_H_
#define _GTE_ANIMATION_MANAGER_H_

#include "engine.h"
#include "object/engineobject.h"

#include <unordered_map>

namespace GTE
{
	class AnimationManager
	{
		// necessary to trigger lifecycle events
		friend class Engine;

		AnimationManager();
		~AnimationManager();

		// map object IDs of Skeleton objects to their assign animation player
		std::unordered_map<ObjectID, AnimationPlayerSharedPtr> activePlayers;

	public:

		Bool IsCompatible(SkinnedMesh3DRendererConstRef meshRenderer, AnimationConstRef animation) const;
		Bool IsCompatible(SkeletonConstRef skeleton, AnimationConstRef animation) const;

		void Update();

		AnimationPlayerSharedPtr RetrieveOrCreateAnimationPlayer(SkeletonRef target);
		AnimationPlayerSharedPtr RetrieveOrCreateAnimationPlayer(SkinnedMesh3DRendererConstRef renderer);
	};
}

#endif
