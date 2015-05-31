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
		SkeletonRef target;
		// mapping from the object ID's of Animation objects to corresponding AnimationInstance objects
		std::vector<AnimationInstanceRef> registeredAnimations;
		// weights used for animation blending
		std::vector<Real> animationWeights;
		// active animation blending operations
		std::queue<BlendOp*> activeBlendOperations;
		// flags that indicate if the animation at a specified index is the target of a cross fade operation in activeBlendOperations
		std::vector<bool> crossFadeTargets;
		// number of animations currently playing
		Int32 playingAnimationsCount;

		AnimationPlayer(SkeletonRef target);
		~AnimationPlayer();

		void QueueBlendOperation(BlendOp * op);
		BlendOp * GetCurrentBlendOp();
		void ClearBlendOpQueue();

		void Update();
		void UpdateBlendingOperations();
		void CheckWeights();
		void ApplyActiveAnimations();
		void UpdateAnimationsProgress();
		void UpdateAnimationInstanceProgress(AnimationInstanceRef instance);
		void CalculateInterpolatedValues(AnimationInstanceRef instance, UInt32 channel, Vector3& translation, Quaternion& rotation, Vector3& scale) const;
		void CalculateInterpolatedTranslation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
		void CalculateInterpolatedScale(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const;
		void CalculateInterpolatedRotation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const;
		bool CalculateInterpolation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, UInt32& lastIndex, UInt32& nextIndex, Real& interFrameProgress, TransformationCompnent component) const;
		Real GetKeyFrameTime(TransformationCompnent transformationComponent, Int32 frameIndex, const KeyFrameSet& keyFrameSet) const;

		void SetSpeed(UInt32 animationIndex, Real speedFactor);
		void Play(UInt32 animationIndex);
		void Stop(UInt32  animationIndex);
		void Pause(UInt32  animationIndex);
		void Resume(UInt32  animationIndex);

	public:

		void AddAnimation(AnimationRef animation);
		void SetSpeed(AnimationRef animation, Real speedFactor);
		void Play(AnimationRef animation);
		void Stop(AnimationRef animation);
		void Pause(AnimationRef animation);
		void Resume(AnimationRef animation);
		void CrossFade(AnimationRef target, Real duration);
		void CrossFade(AnimationRef target, Real duration, bool queued);

		void SetPlaybackMode(AnimationRef target, PlaybackMode playbackMode);
	};
}

#endif
