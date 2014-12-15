#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationplayer.h"
#include "engine.h"
#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "object/engineobjectmanager.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "geometry/matrix4x4.h"
#include "geometry/transform.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/animation/crossfadeblendop.h"
#include "graphics/animation/blendop.h"
#include "global/global.h"
#include "global/constants.h"
#include "ui/debug.h"
#include "util/time.h"
#include <string>
#include <unordered_map>

/*
 * Single constructor, which initializes member variables.
 */
AnimationPlayer::AnimationPlayer(SkeletonRef target)
{
	ASSERT_RTRN(target.IsValid(),"AnimationPlayer::AnimationPlayer -> Invalid target.");
	this->target = target;
	animationCount = 0;
	playingAnimationsCount = 0;
}

/*
 * Destructor
 */
AnimationPlayer::~AnimationPlayer()
{

}

/*
 * Add a blending operation to the queue of active blending operations.
 */
void AnimationPlayer::QueueBlendOperation(BlendOp * op)
{
	ASSERT_RTRN(op,"AnimationPlayer::QueueBlendOperation -> op is NULL.");
	activeBlendOperations.push(op);
}

/*
 * Get the currently active blend operation, if there is one. If there isn't one,
 * return NULL;
 */
BlendOp * AnimationPlayer::GetCurrentBlendOp()
{
	if(activeBlendOperations.size() > 0)return activeBlendOperations.front();
	return NULL;
}

/*
 * Remove all blending operations from the blending operations queue [activeBlendOperations]
 */
void AnimationPlayer::ClearBlendOpQueue()
{
	if(activeBlendOperations.size() > 0)
	{
		BlendOp * op = activeBlendOperations.front();
		op->OnStoppedEarly();
		activeBlendOperations.pop();
		delete op;
	}

	while(activeBlendOperations.size() > 0)
	{
		BlendOp * op = activeBlendOperations.front();
		activeBlendOperations.pop();
		delete op;
	}
}

/*
 * Update & drive the blending operation that is at the head of the queue of
 * active blending operations.
 */
void AnimationPlayer::UpdateBlending()
{
	// check if there are any active blending operations
	if(activeBlendOperations.size() > 0)
	{
		// retrieve (but don't remove) blending operation at head of queue
		BlendOp * op = activeBlendOperations.front();
		if(op == NULL)
		{
			Debug::PrintWarning("AnimationPlayer::UpdateBlending -> NULL operation found in queue.");
			activeBlendOperations.pop();
			return;
		}

		// trigger OnStart callback for blending operation, if appropriate
		if(!op->HasStarted())
		{
			op->SetStarted(true);
			op->OnStart();
		}

		// update blending operation
		op->Update(animationWeights);

		// trigger OnComplete callback and deallocate blending operation, if appropriate
		if(op->HasCompleted())
		{
			op->OnComplete();
			activeBlendOperations.pop();
			delete op;

		}
	}
}

/*
 * Add up weights of active animations, and calculate unused weight [leftOverWeight]
 */
void AnimationPlayer::CheckWeights()
{
	float leftOverWeight = 1;
	for(unsigned int i = 0; i < registeredAnimations.size(); i++)
	{
		AnimationInstanceRef instance = registeredAnimations[i];

		if(instance.IsValid() && instance->Playing)
		{
			leftOverWeight -= animationWeights[i];
		}
	}
	if(leftOverWeight <0)leftOverWeight = 0;
}

/*
 * Trigger all update sub-operations.
 */
void AnimationPlayer::Update()
{
	// update current blending operation
	UpdateBlending();
	// validate animation weights
	CheckWeights();
	// update the positions of all nodes in the target skeleton based on
	// active animations
	UpdatePositionsFromAnimations();
	// drive the progress of active animations
	UpdateAnimationsProgress();
}

/*
 * Update the positions of all nodes of the target Skeleton object based on the progress of all
 * active animations.
 *
 * This method loops through each node in the target skeleton [target], and for each node it calculates
 * the interpolated translation, rotation, and scale for that node for each active animation. It combines
 * those transformations based on the weight of each active animation stored in member [weights] and
 * applies the final transformation to the node.
 */
void AnimationPlayer::UpdatePositionsFromAnimations()
{
	Vector3 translation;
	Vector3 scale;
	Quaternion rotation;

	// storage for aggregate translation, rotation, and scale
	Vector3 agTranslation;
	Vector3 agScale;
	Quaternion agRotation;

	Quaternion temp;

	// temp use Matrices
	Matrix4x4 rotMatrix;
	Matrix4x4 matrix;

	// keep track of the number of playing animations seen as we loop through all registered animations
	unsigned int playingAnimationsSeen = 0;

	// loop through each node in the target Skeleton object, and calculate the position based on
	// weighted average of positions returned from each active animation
	for(unsigned int node = 0; node < target->GetNodeCount(); node++)
	{
		agScale.Set(0,0,0);
		agTranslation.Set(0,0,0);
		agRotation = Quaternion::Identity;

		playingAnimationsSeen = 0;
		float agWeight = 0;

		// loop through all registered animations
		for(int i = registeredAnimations.size()-1; i >= 0; i--)
		{
			AnimationInstanceRef instance = registeredAnimations[i];

			// include this animation only if it is playing
			if(instance.IsValid() && instance->Playing)
			{
				// retrieve this animation's weight
				float weight = animationWeights[i];
				// if this animation's weight is 0, then ignore it
				if(weight <=0)continue;

				// calculate the translation, rotation, and scale for this animation at the current node
				int mappedChannel = instance->GetChannelMappingForTargetNode(node);
				if(mappedChannel < 0)
					{
						continue;
					}

				CalculateInterpolatedValues(instance, mappedChannel, translation, rotation, scale);

				// calculate aggregate (sum of weights up until this point)
				agWeight += weight;

				// if the number of active animations is 1, indicated by playingAnimationsCount == 1, and its
				// weight is 1, then we simply use the results from CalculateInterpolatedValues() and apply those
				// to the current node
				if(playingAnimationsCount == 1 && weight == 1)
				{
					agTranslation = translation;
					agScale = scale;
					agRotation = rotation;
				}
				else // we have to apply weights across 1 or more animations
				{
					// apply weight to each transformation
					translation.Scale(weight);
					scale.Scale(weight);
					rotation.Set(rotation.x() * weight,rotation.y() * weight,rotation.z() * weight, rotation.w() * weight);

					// if this is the first active animation encountered, set the aggregate translation, rotation and scale
					if(playingAnimationsSeen == 0)
					{
						agTranslation = translation;
						agScale = scale;
						agRotation = rotation;
					}
					// this is not the first active animation encountered, so we additively combine the translation
					// and scale into their aggregate counterparts. we use spherical interpolation to combine the
					// rotation for this animation into the aggregate rotation.
					else
					{
						Vector3::Add(translation, agTranslation, agTranslation);
						Vector3::Add(scale, agScale, agScale);

						if(agWeight != 0)temp = Quaternion::slerp(agRotation, rotation, weight/agWeight);
						else temp = rotation;
						agRotation = temp;
					}
				}

				playingAnimationsSeen++;
			}
		}

		// only apply transformations if they were actually calculated
		if(playingAnimationsCount > 0)
		{
			// if the aggregate weight of all playing animations for the current channel does not add up to 1
			// then we use identity values to make up the difference
			if(agWeight < 1)
			{
				float weightDiff = 1 - agWeight;
				agScale.Set(agScale.x + weightDiff,agScale.y + weightDiff,agScale.z + weightDiff);
				Quaternion::slerp(agRotation, Quaternion::Identity, weightDiff);
			}

			agRotation.normalize();
			rotMatrix = agRotation.rotationMatrix();

			matrix.SetIdentity();
			// apply interpolated scale
			matrix.Scale(agScale.x,agScale.y,agScale.z);
			// apply interpolated rotation
			matrix.PreMultiply(rotMatrix);
			// apply interpolated translation
			matrix.PreTranslate(agTranslation.x, agTranslation.y, agTranslation.z);

			// get the Skeleton node corresponding to the current node index
			SkeletonNode * targetNode = target->GetNodeFromList(node);
			if(targetNode->HasTarget())
			{
				// get the local transform of the target of this node and apply
				// [matrix], which contains the interpolated scale, rotation, and translation
				Transform * localTransform = targetNode->GetLocalTransform();
				if(localTransform != NULL)localTransform->SetTo(matrix);
			}
		}
	}
}

/*
 * Use the current progress of [instance] to find the two closest key frames in the KeyFrameSet specified by [channel].
 * Then interpolate between those two key frames based on where the progress of [instance] lies between them, and store the
 * interpolated translation, rotation, and scale values in [translation], [rotation], and [scale].
 */
void AnimationPlayer::CalculateInterpolatedValues(AnimationInstanceRef instance, unsigned int channel, Vector3& translation, Quaternion& rotation, Vector3& scale) const
{
	KeyFrameSet * frameSet = instance->SourceAnimation->GetKeyFrameSet(channel);
	ASSERT_RTRN(frameSet != NULL, "AnimationPlayer::CalculateInterpolatedValues -> frameSet is NULL.");

	// make sure it's an active KeyFrameSet
	if(frameSet != NULL && frameSet->Used)
	{
		// for each of translation, scale, and rotation, find the two respective key frames between which
		// instance->Progress lies, and interpolate between them based on instance->Progress.
		CalculateInterpolatedTranslation(instance, *frameSet, translation);
		CalculateInterpolatedScale(instance, *frameSet, scale);
		CalculateInterpolatedRotation(instance, *frameSet, rotation);
	}
}

/*
 * Drive the progress of each active animation.
 */
void AnimationPlayer::UpdateAnimationsProgress()
{
	// loop through each registered animation and check if it is active. if it is,
	// call UpdateAnimationInstanceProgress() and pass [instance] to it.
	for(unsigned int i = 0; i < registeredAnimations.size(); i++)
	{
		AnimationInstanceRef instance = registeredAnimations[i];
		if(instance.IsValid() && instance->Playing)
		{
			UpdateAnimationInstanceProgress(instance);
		}
	}
}

/*
 * Drive the progress of [instance].
 */
void AnimationPlayer::UpdateAnimationInstanceProgress(AnimationInstanceRef instance) const
{
	// make sure the animation is active
	if(instance->Playing && !instance->Paused)
	{
		// update animation instance progress
		instance->Progress += Time::GetDeltaTime() * instance->SpeedFactor;

		//TODO: update to either stop or loop based on settings. for now auto-loop.
		if(instance->Progress > instance->Duration)
		{
			instance->Progress = instance->Progress - instance->Duration;
			if(instance->Progress < 0) instance->Progress = 0;
		}

		instance->ProgressTicks = instance->Progress * instance->SourceAnimation->GetTicksPerSecond();
	}
}

/*
 * Use the value of instance->Progress to find the two closest translation key frames in [keyFrameSet]. Then interpolate between the translation
 * values in those two key frames based on where instance->Progress lies between them, and store the result in [vector].
 */
void AnimationPlayer::CalculateInterpolatedTranslation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::CalculateInterpolatedTranslation -> Animation is invalid.");

	unsigned int frameCount = keyFrameSet.TranslationKeyFrames.size();
	ASSERT_RTRN(frameCount > 0, "AnimationPlayer::CalculateInterpolatedTranslation -> Key frame count is zero.");

	unsigned int previousIndex, nextIndex;
	float interFrameProgress;
	bool foundFrames = CalculateInterpolation(instance, keyFrameSet, previousIndex, nextIndex, interFrameProgress, TransformationCompnent::Translation);

	// did we successfully find 2 frames between which to interpolate?
	if(foundFrames)
	{
		const TranslationKeyFrame& nextFrame = keyFrameSet.TranslationKeyFrames[nextIndex];
		const TranslationKeyFrame& previousFrame = keyFrameSet.TranslationKeyFrames[previousIndex];

		vector.x = ((nextFrame.Translation.x - previousFrame.Translation.x) * interFrameProgress) + previousFrame.Translation.x;
		vector.y = ((nextFrame.Translation.y - previousFrame.Translation.y) * interFrameProgress) + previousFrame.Translation.y;
		vector.z = ((nextFrame.Translation.z - previousFrame.Translation.z) * interFrameProgress) + previousFrame.Translation.z;

	}
	else //we did not find 2 frames, so set scale equal to the first frame
	{
		const TranslationKeyFrame& firstFrame = keyFrameSet.TranslationKeyFrames[0];
		vector.Set(firstFrame.Translation.x,firstFrame.Translation.y,firstFrame.Translation.y);
	}
}

/*
 * Use the value of instance->Progress to find the two closest scale key frames in [keyFrameSet]. Then interpolate between the scale
 * values in those two key frames based on where instance->Progress lies between them, and store the result in [vector].
 */
void AnimationPlayer::CalculateInterpolatedScale(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::CalculateInterpolatedScale -> Animation is invalid.");

	unsigned int frameCount = keyFrameSet.ScaleKeyFrames.size();
	ASSERT_RTRN(frameCount > 0, "AnimationPlayer::CalculateInterpolatedScale -> Key frame count is zero.");

	unsigned int previousIndex, nextIndex;
	float interFrameProgress;
	bool foundFrames = CalculateInterpolation(instance, keyFrameSet, previousIndex, nextIndex, interFrameProgress, TransformationCompnent::Scale);

	// did we successfully find 2 frames between which to interpolate?
	if(foundFrames)
	{
		const ScaleKeyFrame& nextFrame = keyFrameSet.ScaleKeyFrames[nextIndex];
		const ScaleKeyFrame& previousFrame = keyFrameSet.ScaleKeyFrames[previousIndex];

		// perform interpolation
		vector.x = ((nextFrame.Scale.x - previousFrame.Scale.x) * interFrameProgress) + previousFrame.Scale.x;
		vector.y = ((nextFrame.Scale.y - previousFrame.Scale.y) * interFrameProgress) + previousFrame.Scale.y;
		vector.z = ((nextFrame.Scale.z - previousFrame.Scale.z) * interFrameProgress) + previousFrame.Scale.z;
	}
	else //we did not find 2 frames, so set scale equal to the first frame
	{
		const ScaleKeyFrame& firstFrame = keyFrameSet.ScaleKeyFrames[0];
		vector.Set(firstFrame.Scale.x,firstFrame.Scale.y,firstFrame.Scale.y);
	}
}

/*
 * Use the value of instance->Progress to find the two closest rotation key frames in [keyFrameSet]. Then interpolate between the rotation
 * values in those two key frames based on where instance->Progress lies between them, and store the result in [rotation].
 */
void AnimationPlayer::CalculateInterpolatedRotation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const
{
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::CalculateInterpolatedRotation -> Animation is invalid.");

	unsigned int frameCount = keyFrameSet.RotationKeyFrames.size();
	ASSERT_RTRN(frameCount > 0, "AnimationPlayer::CalculateInterpolatedRotation -> Key frame count is zero.");

	unsigned int previousIndex, nextIndex;
	float interFrameProgress;
	bool foundFrames = CalculateInterpolation(instance, keyFrameSet, previousIndex, nextIndex, interFrameProgress, TransformationCompnent::Rotation);

	// did we successfully find 2 frames between which to interpolate?
	if(foundFrames)
	{
		const RotationKeyFrame& nextFrame = keyFrameSet.RotationKeyFrames[nextIndex];
		const RotationKeyFrame& previousFrame = keyFrameSet.RotationKeyFrames[previousIndex];

		// perform spherical interpolation between the two Quaternions
		Quaternion a(previousFrame.Rotation.x(),previousFrame.Rotation.y(),previousFrame.Rotation.z(),previousFrame.Rotation.w());
		Quaternion b(nextFrame.Rotation.x(),nextFrame.Rotation.y(),nextFrame.Rotation.z(),nextFrame.Rotation.w());
		Quaternion quatOut= Quaternion::slerp(a,b, interFrameProgress);
		rotation.Set(quatOut.x(),quatOut.y(),quatOut.z(), quatOut.w());
	}
	else //we did not find 2 frames, so set rotation equal to the first frame
	{
		const RotationKeyFrame& firstFrame = keyFrameSet.RotationKeyFrames[0];
		rotation.Set(firstFrame.Rotation.x(),firstFrame.Rotation.y(),firstFrame.Rotation.z(), firstFrame.Rotation.w());
	}
}

/*
 * This method uses the value of instance->Progress to find the two closest key frames in [keyFrameSet], of the type specified by [component]
 * and then stores the indices of those key frames in [previousIndex] and [nextIndex]. Then it uses instance->Progress to determine how far from [lastIndex]
 * to [nextIndex] the animation currently is, and stores that value in [interFrameProgress] (range: 0 to 1).
 */
bool AnimationPlayer::CalculateInterpolation(AnimationInstanceRef instance, const KeyFrameSet& keyFrameSet, unsigned int& previousIndex, unsigned int& nextIndex, float& interFrameProgress, TransformationCompnent component) const
{
	ASSERT(instance.IsValid(), "AnimationPlayer::CalculateInterpolation -> Animation is invalid.", false);

	unsigned int frameCount = 0;
	float progress = instance->Progress;
	float duration = instance->Duration;

	// get the correct frame count, which depends on [component]
	if(component == TransformationCompnent::Translation)frameCount = keyFrameSet.TranslationKeyFrames.size();
	else if(component == TransformationCompnent::Rotation)frameCount = keyFrameSet.RotationKeyFrames.size();
	else if(component == TransformationCompnent::Scale)frameCount = keyFrameSet.ScaleKeyFrames.size();
	else return false;

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		KeyFrame * previousFrame = NULL;
		KeyFrame * nextFrame = NULL;

		float keyRealTime = 0;

		// get the correct time stamp for this frame, which depends on [component]
		if(component == TransformationCompnent::Translation)
			keyRealTime = keyFrameSet.TranslationKeyFrames[f].RealTime;
		else if(component == TransformationCompnent::Rotation)
			 keyRealTime = keyFrameSet.RotationKeyFrames[f].RealTime;
		else if(component == TransformationCompnent::Scale)
			keyFrameSet.ScaleKeyFrames[f].RealTime;

		// if the RealTime value for this key frame is greater than [progress], then the previous key frame and the current key frame
		// are the frames we want
		if(keyRealTime > progress  || f == frameCount-1)
		{
			previousIndex = 0;
			if(f > 0)previousIndex = f-1;
			nextIndex = f;

			// flag that indicates we need to interpolate from the last frame to the first frame
			bool overShoot = false;

			// if f==frameCount-1 and keyRealTime <= progress, then we have reached the last frame and progress has moved
			// beyond it. this means we need to interpolate between the last frame and the first frame (for smoothed animation looping).
			if(f == frameCount-1 && keyRealTime <= progress)
			{
				previousIndex = f;
				nextIndex = 0;
				overShoot = true;
			}

			// get pointers to the previous frame and the next frame
			if(component == TransformationCompnent::Translation)
			{
				const TranslationKeyFrame& nextTranslationFrame = keyFrameSet.TranslationKeyFrames[nextIndex];
				const TranslationKeyFrame& lastTranslationFrame = keyFrameSet.TranslationKeyFrames[previousIndex];
				nextFrame = const_cast<TranslationKeyFrame *>(&nextTranslationFrame);
				previousFrame = const_cast<TranslationKeyFrame *>(&lastTranslationFrame);
			}
			else if(component == TransformationCompnent::Rotation)
			{
				const RotationKeyFrame& nextRotationFrame = keyFrameSet.RotationKeyFrames[nextIndex];
				const RotationKeyFrame& lastRotationFrame = keyFrameSet.RotationKeyFrames[previousIndex];
				nextFrame = const_cast<RotationKeyFrame *>(&nextRotationFrame);
				previousFrame = const_cast<RotationKeyFrame *>(&lastRotationFrame);
			}
			else if(component == TransformationCompnent::Scale)
			{
				const ScaleKeyFrame& nextScaleFrame = keyFrameSet.ScaleKeyFrames[nextIndex];
				const ScaleKeyFrame& lastScaleFrame = keyFrameSet.ScaleKeyFrames[previousIndex];
				nextFrame = const_cast<ScaleKeyFrame *>(&nextScaleFrame);
				previousFrame = const_cast<ScaleKeyFrame *>(&lastScaleFrame);
			}

			// calculate local progress between [previous] and [nextFrame]
			float interFrameTimeDelta = nextFrame->RealTime - previousFrame->RealTime;
			if(overShoot)  interFrameTimeDelta = duration -previousFrame->RealTime;

			float interFrameElapsed = progress - previousFrame->RealTime;
			interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			return true;
		}
	}

	return false;
}

/*
 * Add an Animation object [animation] to this player. The Animation must be compatible with
 * the existing target of this player.
 */
void AnimationPlayer::AddAnimation(AnimationRef animation)
{
	AnimationManager * animationManager = Engine::Instance()->GetAnimationManager();
	ASSERT_RTRN(animationManager != NULL,"AnimationPlayer::CreateAnimationInstance -> Animation manager is NULL.");

	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::CreateAnimationInstance -> Animation is invalid.");

	// verify compatibility with [target]
	ASSERT_RTRN(animationManager->IsCompatible(target,animation), "AnimationPlayer::CreateAnimationInstance -> Skeleton is not compatible with animation.");

	AnimationInstanceRef instance;

	// make sure an instance of [animation] does not already exist for this player
	if(animationIndexMap.find(animation->GetObjectID()) == animationIndexMap.end())
	{
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
		AnimationInstanceRef instance = objectManager->CreateAnimationInstance(target, animation);

		ASSERT_RTRN(instance.IsValid(), " AnimationPlayer::CreateAnimationInstance -> Unable to create animation instance.");

		bool initSuccess = instance->Init();
		ASSERT_RTRN(initSuccess,"AnimationPlayer::CreateAnimationInstance -> Unable to initialize animation instance.");

		registeredAnimations.push_back(instance);
		animationWeights.push_back(0);
		crossFadeTargets.push_back(false);

		animationIndexMap[animation->GetObjectID()] = animationCount;
		animationCount++;
	}
}

/*
 * Set the normalized speed of playback of [animation] to [speedFactor].
 */
void AnimationPlayer::SetSpeed(AnimationRef animation, float speedFactor)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::SetSpeed -> Animation is invalid.");
	if(animationIndexMap.find(animation->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[animation->GetObjectID()];
		SetSpeed(targetIndex, speedFactor);
	}
}

void AnimationPlayer::SetSpeed(unsigned int animationIndex, float speedFactor)
{
	ASSERT_RTRN(animationIndex < animationCount, "AnimationPlayer::SetSpeed -> invalid animation index.");

	AnimationInstanceRef instance = registeredAnimations[animationIndex];
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::SetSpeed -> Target animation is invalid.");

	instance->SpeedFactor = speedFactor;
}

/*
 * Start or resume playback of [animation] on the target of this player.
 */
void AnimationPlayer::Play(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Play -> Animation is invalid.");
	if(animationIndexMap.find(animation->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[animation->GetObjectID()];
		Play(targetIndex);
	}
}

/*
 * Start or resume playback of registered animation at [animationIndex].
 */
void AnimationPlayer::Play(unsigned int animationIndex)
{
	ASSERT_RTRN(animationIndex < animationCount, "AnimationPlayer::Play -> invalid animation index.");

	float tempLeftOver = 1;
	for(unsigned int i = 0; i < registeredAnimations.size(); i++)
	{
		AnimationInstanceRef instance = registeredAnimations[i];

		if(i != animationIndex)
		{
			instance->Stop();
			animationWeights[i] = 0;
		}
		else
		{
			animationWeights[i] = 1;
			instance->Play();
		}

		tempLeftOver -= animationWeights[1];
	}
	if(tempLeftOver < 0)tempLeftOver = 0;
	playingAnimationsCount = 1;
}

/*
 * Stop playback of [animation] on this player.
 */
void AnimationPlayer::Stop(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Stop -> Animation is invalid.");
	if(animationIndexMap.find(animation->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Stop -> invalid animation index found in index map.");

		Stop(targetIndex);
	}
}

/*
 * Stop playback of registered animation at [animationIndex].
 */
void AnimationPlayer::Stop(unsigned int  animationIndex)
{
	ASSERT_RTRN(animationIndex < animationCount, "AnimationPlayer::Stop -> invalid animation index.");

	AnimationInstanceRef instance = registeredAnimations[animationIndex];
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Stop -> Target animation is invalid.");

	if(instance->Playing)playingAnimationsCount--;
	instance->Stop();

	animationWeights[animationIndex] = 0;
}

/*
 * Pause playback of [animation] on this player.
 */
void AnimationPlayer::Pause(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Pause -> Animation is invalid.");
	if(animationIndexMap.find(animation->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Pause -> invalid animation index found in index map.");

		Pause(targetIndex);
	}
}

/*
 * Pause playback of registered animation at [animationIndex].
 */
void AnimationPlayer::Pause(unsigned int  animationIndex)
{
	ASSERT_RTRN(animationIndex < animationCount, "AnimationPlayer::Pause -> invalid animation index.");

	AnimationInstanceRef instance = registeredAnimations[animationIndex];
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Pause -> Target animation is invalid.");

	instance->Pause();
}

/*
 * Resume playback of [animation] on this player.
 */
void AnimationPlayer::Resume(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Resume -> Animation is invalid.");
	if(animationIndexMap.find(animation->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Resume -> invalid animation index found in index map.");

		AnimationInstanceRef instance = registeredAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Resume -> Target animation is invalid.");

		instance->Play();
	}
}

/*
 * Resume playback of registered animation at [animationIndex].
 */
void AnimationPlayer::Resume(unsigned int  animationIndex)
{
	ASSERT_RTRN(animationIndex < animationCount, "AnimationPlayer::Resume -> invalid animation index.");

	AnimationInstanceRef instance = registeredAnimations[animationIndex];
	ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Resume -> Target animation is invalid.");

	instance->Play();
}

void AnimationPlayer::CrossFade(AnimationRef target, float duration)
{
	CrossFade(target, duration, false);
}

void AnimationPlayer::CrossFade(AnimationRef target, float duration, bool queued)
{
	if(animationIndexMap.find(target->GetObjectID()) != animationIndexMap.end())
	{
		unsigned int targetIndex = animationIndexMap[target->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::CrossFade -> invalid animation index found in index map.");

		AnimationInstanceRef instance = registeredAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::CrossFade -> Target animation is invalid.");

		// if the current blending operation is a crossfade with the same target, then do nothing
		if(crossFadeTargets[targetIndex] == 1)return;

		CrossFadeBlendOp * blendOp = new CrossFadeBlendOp(duration, targetIndex);
		ASSERT_RTRN(blendOp, "AnimationPlayer::CrossFade -> Unable to allocate new CrossFadeBlendOp object.");

		bool initSuccess = blendOp->Init(animationWeights);
		if(!initSuccess)
		{
			Debug::PrintError("AnimationPlayer::CrossFade -> Unable to init new CrossFadeBlendOp object.");
			delete blendOp;
			return;
		}

		crossFadeTargets[targetIndex] = 1;
		blendOp->SetOnStartCallback([targetIndex, this](CrossFadeBlendOp * op)
		{
			AnimationInstanceRef instance = registeredAnimations[targetIndex];
			if(!instance.IsValid())
			{
				Debug::PrintError("AnimationPlayer::CrossFade::SetOnStartCallback -> Invalid target animation.");
				return;
			}

			bool initSuccess = op->Init(animationWeights);
			if(!initSuccess)
			{
				Debug::PrintError("AnimationPlayer::CrossFade::SetOnStartCallback -> Unable to init CrossFadeBlendOp object.");
				return;
			}

			if(!instance->Playing)
			{
				playingAnimationsCount++;
				instance->Reset();
				instance->Play();
			}
		});

		blendOp->SetOnStoppedEarlyCallback([targetIndex, this](CrossFadeBlendOp * op)
		{
			crossFadeTargets[targetIndex] = 0;
		});

		blendOp->SetOnCompleteCallback([targetIndex, this](CrossFadeBlendOp * op)
		{
			for(unsigned int i = 0; i < registeredAnimations.size(); i++)
			{
				if(i != targetIndex)
				{
					AnimationInstanceRef instance = registeredAnimations[i];
					if(!instance.IsValid())
					{
						Debug::PrintWarning("AnimationPlayer::CrossFade::OnCompleteCallback -> Invalid animation found.");
						continue;
					}

					Stop(i);
					animationWeights[i] = 0;
				}
				else
				{
					animationWeights[i] = 1;
				}
			}

			crossFadeTargets[targetIndex] = 0;
			playingAnimationsCount = 1;
		});

		// clear the current queue if [queued] == false
		if(!queued)ClearBlendOpQueue();

		QueueBlendOperation(blendOp);
	}
}
