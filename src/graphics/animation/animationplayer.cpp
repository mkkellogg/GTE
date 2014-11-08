#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationplayer.h"
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
	indexCount = 0;
}

/*
 * Destructor
 */
AnimationPlayer::~AnimationPlayer()
{

}

/*
 * Update all animation instances to reflect the time that has past since the last call to Drive().
 */
void AnimationPlayer::Drive()
{
	for(unsigned int i = 0; i < activeAnimations.size(); i++)
	{
		AnimationInstanceRef instance = activeAnimations[i];

		if(instance.IsValid())
		{
			// TODO: Implement animation blending.
			// For now simply update instance with weight of 1.
			UpdateAnimationInstance(instance);
		}
	}
}

/*
 * Advance [instance] according to the amount of time that has elapsed since the last call to UpdateAnimationInstance()
 */
void AnimationPlayer::UpdateAnimationInstance(AnimationInstanceRef instance) const
{
	ASSERT_RTRN(instance.IsValid(), " AnimationPlayer::UpdateAnimationInstance -> Animation instance is invalid.");

	// only update if playing
	if(instance->IsPlaying())
	{
		// temp use Matrices
		Matrix4x4 rotMatrix;
		Matrix4x4 matrix;

		// loop through each KeyFrameSet in animation and advance according to the instance's progress
		for(unsigned int state = 0; state < instance->StateCount; state++)
		{
			unsigned int frameSetIndex = state;
			KeyFrameSet * frameSet = instance->SourceAnimation->GetKeyFrameSet(frameSetIndex);

			// make sure it's an active KeyFrameSet
			if(frameSet != NULL && frameSet->Used)
			{
				Vector3 translation;
				Vector3 scale;
				Quaternion rotation;

				// for each of translation, scale, and rotation, find the two respective key frames between which
				// instance->Progress lies, and interpolate between them based on instance->Progress.
				CalculateInterpolatedTranslation(instance->Progress, *frameSet, translation);
				CalculateInterpolatedScale(instance->Progress, *frameSet, scale);
				CalculateInterpolatedRotation(instance->Progress, *frameSet, rotation);
				rotMatrix = rotation.rotationMatrix();

				matrix.SetIdentity();
				// apply interpolated scale
				matrix.Scale(scale.x,scale.y,scale.z);
				// apply interpolated rotation
				matrix.PreMultiply(&rotMatrix);
				// apply interpolated translation
				matrix.PreTranslate(translation.x, translation.y, translation.z);

				// get the Skeleton node corresponding to this KeyFrameSet
				SkeletonNode * targetNode = instance->Target->GetNodeFromList(frameSetIndex);
				if(targetNode->HasTarget())
				{
					// get the local transform of the target of this node and apply
					// [matrix], which contains the interpolated scale, rotation, and translation
					Transform * localTransform = targetNode->GetLocalTransform();
					if(localTransform != NULL)
					{
						localTransform->SetTo(&matrix);
					}
				}
			}
		}

		// update animation instance progress
		float deltaTime = Time::GetDeltaTime();
		instance->Progress += deltaTime;

		//TODO: update to either stop or loop based on settings. for now auto-loop.
		if(instance->Progress >= instance->Duration)
		{
			instance->Progress = instance->Progress - instance->Duration;
			if(instance->Progress < 0) instance->Progress = 0;
		}

		instance->ProgressTicks = instance->Progress * instance->SourceAnimation->GetTicksPerSecond();
	}
}

/*
 * Use the value of [progress] to find the two closest translation key frames in [keyFrameSet]. Then interpolate between the translation
 * values in those two key frames based on where [progress] lies between them, and store the result in [vector].
 */
void AnimationPlayer::CalculateInterpolatedTranslation(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.TranslationKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		const TranslationKeyFrame& keyFrame = keyFrameSet.TranslationKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			const TranslationKeyFrame& lastFrame = keyFrameSet.TranslationKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform interpolation
			vector.x = ((keyFrame.Translation.x - lastFrame.Translation.x) * interFrameProgress) + lastFrame.Translation.x;
			vector.y = ((keyFrame.Translation.y - lastFrame.Translation.y) * interFrameProgress) + lastFrame.Translation.y;
			vector.z = ((keyFrame.Translation.z - lastFrame.Translation.z) * interFrameProgress) + lastFrame.Translation.z;

			foundFrame = true;
			break;
		}
	}

	if(!foundFrame && frameCount > 0)
	{
		const TranslationKeyFrame& lastFrame = keyFrameSet.TranslationKeyFrames[frameCount-1];
		vector.Set(lastFrame.Translation.x,lastFrame.Translation.y,lastFrame.Translation.y);
	}
}

/*
 * Use the value of [progress] to find the two closest scale key frames in [keyFrameSet]. Then interpolate between the scale
 * values in those two key frames based on where [progress] lies between them, and store the result in [vector].
 */
void AnimationPlayer::CalculateInterpolatedScale(float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.ScaleKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		const ScaleKeyFrame& keyFrame = keyFrameSet.ScaleKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			const ScaleKeyFrame& lastFrame = keyFrameSet.ScaleKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform interpolation
			vector.x = ((keyFrame.Scale.x - lastFrame.Scale.x) * interFrameProgress) + lastFrame.Scale.x;
			vector.y = ((keyFrame.Scale.y - lastFrame.Scale.y) * interFrameProgress) + lastFrame.Scale.y;
			vector.z = ((keyFrame.Scale.z - lastFrame.Scale.z) * interFrameProgress) + lastFrame.Scale.z;

			foundFrame = true;
			break;
		}
	}

	if(!foundFrame && frameCount > 0)
	{
		const ScaleKeyFrame& lastFrame = keyFrameSet.ScaleKeyFrames[frameCount-1];
		vector.Set(lastFrame.Scale.x,lastFrame.Scale.y,lastFrame.Scale.y);
	}
}

/*
 * Use the value of [progress] to find the two closest rotation key frames in [keyFrameSet]. Then interpolate between the rotation
 * values in those two key frames based on where [progress] lies between them, and store the result in [rotation].
 */
void AnimationPlayer::CalculateInterpolatedRotation(float progress, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.RotationKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		const RotationKeyFrame& keyFrame = keyFrameSet.RotationKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			const RotationKeyFrame& lastFrame = keyFrameSet.RotationKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform spherical interpolation between the two Quaternions
			Quaternion a(lastFrame.Rotation.x(),lastFrame.Rotation.y(),lastFrame.Rotation.z(),lastFrame.Rotation.w());
			Quaternion b(keyFrame.Rotation.x(),keyFrame.Rotation.y(),keyFrame.Rotation.z(),keyFrame.Rotation.w());
			Quaternion quatOut= Quaternion::slerp(a,b, interFrameProgress);
			rotation.Set(quatOut.x(),quatOut.y(),quatOut.z(), -quatOut.w());

			foundFrame = true;
			break;
		}
	}

	if(!foundFrame && frameCount > 0)
	{
		const RotationKeyFrame& lastFrame = keyFrameSet.RotationKeyFrames[frameCount-1];
		rotation.Set(lastFrame.Rotation.x(),lastFrame.Rotation.y(),lastFrame.Rotation.z(), -lastFrame.Rotation.w());
	}
}

/*
 * Add an Animation object [animation] to this player. The Animation must be compatible with
 * the existing target of this player.
 */
void AnimationPlayer::AddAnimation(AnimationRef animation)
{
	AnimationManager * animationManager = AnimationManager::Instance();
	ASSERT_RTRN(animationManager != NULL,"AnimationPlayer::CreateAnimationInstance -> Animation manager is NULL.");

	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::CreateAnimationInstance -> Animation is invalid.");

	// verify compatibility with [target]
	ASSERT_RTRN(animationManager->IsCompatible(target,animation), "AnimationPlayer::CreateAnimationInstance -> Skeleton is not compatible with animation.");

	AnimationInstanceRef instance;

	// make sure an instance of [animation] does not already exist for this player
	if(activeAnimationIndices.find(animation->GetObjectID()) == activeAnimationIndices.end())
	{
		EngineObjectManager * objectManager = EngineObjectManager::Instance();
		AnimationInstanceRef instance = objectManager->CreateAnimationInstance(target, animation);

		ASSERT_RTRN(instance.IsValid(), " AnimationPlayer::CreateAnimationInstance -> Unable to create animation instance.");

		bool initSuccess = instance->Init();
		ASSERT_RTRN(initSuccess,"AnimationPlayer::CreateAnimationInstance -> Unable to initialize animation instance.");

		activeAnimations.push_back(instance);

		activeAnimationIndices[animation->GetObjectID()] = indexCount;
		indexCount++;
	}
}

/*
 * Start or resume playback of [animation] on the target of this player.
 */
void AnimationPlayer::Play(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Play -> Animation is invalid.");
	if(activeAnimationIndices.find(animation->GetObjectID()) != activeAnimationIndices.end())
	{
		unsigned int index = activeAnimationIndices[animation->GetObjectID()];
		AnimationInstanceRef instance = activeAnimations[index];
		instance->Play();
	}
}

/*
 * Stop playback of [animation] on this player.
 */
void AnimationPlayer::Stop(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Stop -> Animation is invalid.");
	if(activeAnimationIndices.find(animation->GetObjectID()) != activeAnimationIndices.end())
	{
		unsigned int index = activeAnimationIndices[animation->GetObjectID()];
		AnimationInstanceRef instance = activeAnimations[index];
		instance->Stop();
	}
}

/*
 * Pause playback of [animation] on this player.
 */
void AnimationPlayer::Pause(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Pause -> Animation is invalid.");
	if(activeAnimationIndices.find(animation->GetObjectID()) != activeAnimationIndices.end())
	{
		unsigned int index = activeAnimationIndices[animation->GetObjectID()];
		AnimationInstanceRef instance = activeAnimations[index];
		instance->Pause();
	}
}
