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

#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include "object/enginetypes.h"
#include "object/engineobject.h"
#include <unordered_map>

class AnimationManager
{
	AnimationManager();
    ~AnimationManager();

    // singleton instance
    static AnimationManager * instance;
    // map object IDs of Skeleton objects to their assign animation player
    std::unordered_map<ObjectID, AnimationPlayerRef> activePlayers;

	public :

    static AnimationManager * Instance();

    bool IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation) const;
    bool IsCompatible(SkeletonRef skeleton, AnimationRef animation) const;

    void Drive();

    AnimationPlayerRef RetrieveOrCreateAnimationPlayer(SkeletonRef target);
    AnimationPlayerRef RetrieveOrCreateAnimationPlayer(SkinnedMesh3DRendererRef renderer);
};

#endif
