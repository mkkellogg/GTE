#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

//forward declarations

#include "object/enginetypes.h"
#include <vector>
#include <string>

class AnimationManager
{
	protected:

	public :

	AnimationManager();
    ~AnimationManager();

    bool IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation);
    bool IsCompatible(SkeletonRef skeleton, AnimationRef animation);

   // AnimationInstanceRef CreateAnimationInstance()
};

#endif
