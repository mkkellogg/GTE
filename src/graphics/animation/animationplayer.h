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
class BlendOp;

#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "keyframeset.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

class AnimationPlayer
{
	friend class EngineObjectManager;
	friend class AnimationManager;

	// number of animations being handled by this player
	unsigned int animationCount;
	// mapping from object ID's of Animation objects to respective indices in member arrays/vectors
	std::unordered_map<ObjectID, unsigned int> activeAnimationIndices;
	// target of all animations managed by this player
	SkeletonRef target;
	// mapping from the object ID's of Animation objects to corresponding AnimationInstance objects
	std::vector<AnimationInstanceRef> activeAnimations;
	// weights used for animation blending
	std::vector<float> weights;
	// if the sum of [weights] is less than 1, the difference is stored in [leftOverWeight]
	float leftOverWeight;
	// active animation blending operations
	std::queue<BlendOp*> activeBlendOperations;

	AnimationPlayer(SkeletonRef target);
	~AnimationPlayer();

	void QueueBlendOperation(BlendOp * op);
	void Update();
	void UpdateBlending();
	void CheckWeights();
	void UpdateAnimations();
	void UpdateAnimationInstance(AnimationInstanceRef instance) const;
	void CalculateInterpolatedTranslation(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
	void CalculateInterpolatedScale(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
	void CalculateInterpolatedRotation(float progress, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const;

	public :

    void AddAnimation(AnimationRef animation);
	void Play(AnimationRef animation);
	void Stop(AnimationRef animation);
	void Pause(AnimationRef animation);
	void Resume(AnimationRef animation);
	void CrossFade(AnimationRef target, float duration);
};

#endif
