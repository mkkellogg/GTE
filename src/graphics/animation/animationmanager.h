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

#include "object/enginetypes.h"
#include "object/engineobject.h"
#include <unordered_map>

namespace GTE
{
	class AnimationManager
	{
		friend class Engine;

		AnimationManager();
		~AnimationManager();

		// map object IDs of Skeleton objects to their assign animation player
		std::unordered_map<ObjectID, AnimationPlayerSharedPtr> activePlayers;

	public:

		Bool IsCompatible(SkinnedMesh3DRendererSharedConstPtr meshRenderer, AnimationSharedConstPtr animation) const;
		Bool IsCompatible(SkeletonSharedConstPtr skeleton, AnimationSharedConstPtr animation) const;

		void Update();

		AnimationPlayerSharedPtr RetrieveOrCreateAnimationPlayer(SkeletonSharedPtr target);
		AnimationPlayerSharedPtr RetrieveOrCreateAnimationPlayer(SkinnedMesh3DRendererSharedConstPtr renderer);
	};
}

#endif
