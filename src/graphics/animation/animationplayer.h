/*********************************************
*
* class: AnimationPlayer
*
* author: Mark Kellogg
*
* This class manages playback of all animations for a single Skeleton target.
*
***********************************************/

#ifndef _GTE_ANIMATION_PLAYER_H_
#define _GTE_ANIMATION_PLAYER_H_

#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "keyframeset.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

namespace GTE
{
	//forward declarations
	class Transform;
	class SkeletonNode;
	class BlendOp;

	enum class TransformationCompnent
	{
		Translation = 0,
		Rotation = 1,
		Scale = 2
	};

	enum class PlaybackMode
	{
		Repeat = 0,
		Clamp = 1,
		PingPong = 2
	};

	class AnimationPlayer
	{
		friend class EngineObjectManager;
		friend class AnimationManager;

		// number of animations being handled by this player
		UInt32 animationCount;
		// mapping from object ID's of Animation objects to respective indices in member arrays/vectors
		std::unordered_map<ObjectID, UInt32> animationIndexMap;
		// target of all animations managed by this player
		SkeletonSharedPtr target;
		// mapping from the object ID's of Animation objects to corresponding AnimationInstance objects
		std::vector<AnimationInstanceSharedPtr> registeredAnimations;
		// weights used for animation blending
		std::vector<Real> animationWeights;
		// active animation blending operations
		std::queue<BlendOp*> activeBlendOperations;
		// flags that indicate if the animation at a specified index is the target of a cross fade operation in activeBlendOperations
		std::vector<Bool> crossFadeTargets;
		// number of animations currently playing
		Int32 playingAnimationsCount;

		AnimationPlayer(SkeletonSharedPtr target);
		~AnimationPlayer();

		void QueueBlendOperation(BlendOp * op);
		BlendOp * GetCurrentBlendOp();
		void ClearBlendOpQueue();

		void Update();
		void UpdateBlendingOperations();
		void CheckWeights();
		void ApplyActiveAnimations();
		void UpdateAnimationsProgress();
		void UpdateAnimationInstanceProgress(AnimationInstanceSharedPtr instance) const;
		void CalculateInterpolatedValues(AnimationInstanceSharedConstPtr instance, UInt32 channel, Vector3& translation, Quaternion& rotation, Vector3& scale) const;
		void CalculateInterpolatedTranslation(AnimationInstanceSharedConstPtr instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
		void CalculateInterpolatedScale(AnimationInstanceSharedConstPtr instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
		void CalculateInterpolatedRotation(AnimationInstanceSharedConstPtr instance, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const;
		Bool CalculateInterpolation(AnimationInstanceSharedConstPtr instance, const KeyFrameSet& keyFrameSet, UInt32& lastIndex, UInt32& nextIndex, Real& interFrameProgress, TransformationCompnent component) const;
		Real GetKeyFrameTime(TransformationCompnent transformationComponent, Int32 frameIndex, const KeyFrameSet& keyFrameSet) const;

		void SetSpeed(UInt32 animationIndex, Real speedFactor);
		void Play(UInt32 animationIndex);
		void Stop(UInt32  animationIndex);
		void Pause(UInt32  animationIndex);
		void Resume(UInt32  animationIndex);

	public:

		void AddAnimation(AnimationSharedConstPtr animation);
		void SetSpeed(AnimationSharedConstPtr animation, Real speedFactor);
		void Play(AnimationSharedConstPtr animation);
		void Stop(AnimationSharedConstPtr animation);
		void Pause(AnimationSharedConstPtr animation);
		void Resume(AnimationSharedConstPtr animation);
		void CrossFade(AnimationSharedConstPtr target, Real duration);
		void CrossFade(AnimationSharedConstPtr target, Real duration, Bool queued);
		void SetPlaybackMode(AnimationSharedConstPtr target, PlaybackMode playbackMode);
	};
}

#endif
