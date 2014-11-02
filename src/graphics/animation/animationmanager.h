#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

//forward declarations

#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "keyframeset.h"
#include <vector>
#include <unordered_map>
#include <string>

class AnimationManager
{
	AnimationManager();
    ~AnimationManager();

    static AnimationManager * instance;
    std::unordered_map<unsigned int, std::vector<AnimationInstanceRef>> activeAnimations;

    void UpdateAnimationInstance(AnimationInstanceRef instance);

    void CalculateInterpolatedTranslation(float progress, float duration, KeyFrameSet& keyFrameSet, Vector3& vector);
    void CalculateInterpolatedScale(float progress, float duration, KeyFrameSet& keyFrameSet, Vector3& vector);
    void CalculateInterpolatedRotation(float progress, float duration, KeyFrameSet& keyFrameSet, Quaternion& rotation);

	public :

    static AnimationManager * Instance();

    bool IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation);
    bool IsCompatible(SkeletonRef skeleton, AnimationRef animation);

    void Drive();
    AnimationInstanceRef CreateAnimationInstance(SkeletonRef skeleton, AnimationRef animation);
    AnimationInstanceRef CreateAnimationInstance(SkinnedMesh3DRendererRef renderer, AnimationRef animation);
};

#endif
