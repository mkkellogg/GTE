
/*********************************************
*
* class: AnimationPlayer
*
* author: Mark Kellogg
*
* This class manages playback of all animations for a single Skeleton target.
*
***********************************************/

#ifndef _ANIMATION_PLAYER_H_
#define _ANIMATION_PLAYER_H_

//forward declarations
class Transform;
class SkeletonNode;

#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "keyframeset.h"
#include <vector>
#include <unordered_map>
#include <string>

class AnimationPlayer
{
	friend class EngineObjectManager;
	friend class AnimationManager;

	// target of all animations managed by this player
	SkeletonRef target;
	// mapping from the object ID's of Animation objects to corresponding AnimationInstance objects
	std::unordered_map<unsigned int, AnimationInstanceRef> activeAnimations;
	// weights used for animation blending
	std::vector<float> weights;

	AnimationPlayer(SkeletonRef target);
	~AnimationPlayer();

	void Drive();

	void UpdateAnimationInstance(AnimationInstanceRef instance) const;
	void CalculateInterpolatedTranslation(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
	void CalculateInterpolatedScale(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
	void CalculateInterpolatedRotation(float progress, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const;

	public :

    void AddAnimation(AnimationRef animation);
	void Play(AnimationRef animation);
	void Stop(AnimationRef animation);
	void Pause(AnimationRef animation);
};

#endif
