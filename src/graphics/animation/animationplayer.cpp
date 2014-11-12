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
	leftOverWeight = 1;
	playingAnimationsCount = 0;
}

/*
 * Destructor
 */
AnimationPlayer::~AnimationPlayer()
{

}

void AnimationPlayer::QueueBlendOperation(BlendOp * op)
{
	ASSERT_RTRN(op,"AnimationPlayer::QueueBlendOperation -> op is NULL.");
	activeBlendOperations.push(op);
}

void AnimationPlayer::UpdateBlending()
{
	if(activeBlendOperations.size() > 0)
	{
		BlendOp * op = activeBlendOperations.front();

		if(op == NULL)
		{
			Debug::PrintWarning("AnimationPlayer::UpdateBlending -> NULL operation found in queue.");
			activeBlendOperations.pop();
			return;
		}

		if(!op->IsStarted())
		{
			op->OnStart();
			op->SignalStarted();
		}

		//printf("cross-fading: %f% \n",op->GetNormalizedProgress() * 100);
		op->Update(weights);

		if(op->IsComplete())
		{
			activeBlendOperations.pop();
			op->OnComplete();
			delete op;
		}
	}
}

void AnimationPlayer::CheckWeights()
{
	leftOverWeight = 1;
	for(unsigned int i = 0; i < activeAnimations.size(); i++)
	{
		AnimationInstanceRef instance = activeAnimations[i];

		if(instance.IsValid() && instance->Playing)
		{
			leftOverWeight -= weights[i];
		}
	}
	if(leftOverWeight <0)leftOverWeight = 0;
}

/*
 * Update all animation instances to reflect the time that has past since the last call to Drive().
 */
void AnimationPlayer::Update()
{
	UpdateBlending();
	CheckWeights();
	UpdateAnimations();
}

void AnimationPlayer::UpdateAnimations()
{
	Vector3 translation;
	Vector3 scale;
	Quaternion rotation;
	aiQuaternion aiRotation;

	Vector3 agTranslation;
	Vector3 agScale;
	Quaternion agRotation;
	Quaternion temp;

	// temp use Matrices
	Matrix4x4 rotMatrix;
	Matrix4x4 matrix;

	unsigned int playingAnimationsSeen = 0;

	// loop through each node in the target Skeleton object calculated the position based on weighted average
	// of values returned from each active animation
	for(unsigned int node = 0; node < target->GetNodeCount(); node++)
	{
		playingAnimationsSeen = 0;
		float agWeight = 0;
		for(int i = activeAnimations.size()-1; i >= 0; i--)
		{
			AnimationInstanceRef instance = activeAnimations[i];
			if(instance.IsValid() && instance->Playing)
			{
				float weight = weights[i];
				if(weight <=0)continue;

				if(playingAnimationsCount == 1)
				{
					UpdateAnimationInstancePositions(instance, node, translation, rotation, scale);
					agTranslation = translation;
					agScale = scale;
					agRotation = rotation;
				}
				else
				{
					agWeight += weight;
					UpdateAnimationInstancePositions(instance, node, translation, rotation, scale);

					translation.Scale(weight);
					scale.Scale(weight);
					rotation.Set(rotation.x() * weight,rotation.y() * weight,rotation.z() * weight, rotation.w() * weight);
					rotation.normalize();

					if(playingAnimationsSeen == 0)
					{
						agTranslation = translation;
						agScale = scale;
						agRotation = rotation;
					}
					else
					{
						Vector3::Add(&translation, &agTranslation, &agTranslation);
						Vector3::Add(&scale, &agScale, &agScale);
						//agRotation =  agRotation * rotation;
						if(agWeight != 0)
							temp = Quaternion::slerp(agRotation, rotation, weight/agWeight);
						else
							temp = rotation;
						agRotation =temp;
					}
				}

				playingAnimationsSeen++;
			}
		}

		rotMatrix = agRotation.rotationMatrix();

		matrix.SetIdentity();
		// apply interpolated scale
		matrix.Scale(agScale.x,agScale.y,agScale.z);
		// apply interpolated rotation
		matrix.PreMultiply(&rotMatrix);
		// apply interpolated translation
		matrix.PreTranslate(agTranslation.x, agTranslation.y, agTranslation.z);

		// get the Skeleton node corresponding to the current node index
		SkeletonNode * targetNode = target->GetNodeFromList(node);
		if(targetNode->HasTarget())
		{
			// get the local transform of the target of this node and apply
			// [matrix], which contains the interpolated scale, rotation, and translation
			Transform * localTransform = targetNode->GetLocalTransform();
			if(localTransform != NULL)localTransform->SetTo(&matrix);
		}
	}

	for(unsigned int i = 0; i < activeAnimations.size(); i++)
	{
		AnimationInstanceRef instance = activeAnimations[i];
		if(instance.IsValid() && instance->Playing)
		{
			UpdateAnimationInstanceProgress(instance);
		}
	}


	if(leftOverWeight > 0)
	{
		//UpdateTargetWithWeightedIdentity(1);
	}
}

void AnimationPlayer::UpdateTargetWithWeightedIdentity(float weight)
{
	ASSERT_RTRN(target.IsValid(), " AnimationPlayer::UpdateTargetWithWeightedIdentity -> Target skeleton is invalid.");

	for(unsigned int i = 0; i < target->GetNodeCount(); i++)
	{
		SkeletonNode * node = target->GetNodeFromList(i);

		if(node != NULL && node->HasTarget())
		{
			Matrix4x4 matrix;
			matrix.MultiplyByScalar(weight);

			Matrix4x4 * mat = const_cast<Matrix4x4*>(node->GetLocalTransform()->GetMatrix());
			mat->Add(&matrix);
		}
	}
}
AnimationInstanceRef _tempInstance;
void AnimationPlayer::UpdateAnimationInstancePositions(AnimationInstanceRef instance, unsigned int state, Vector3& translation, Quaternion& rotation, Vector3& scale) const
{
	unsigned int frameSetIndex = state;
	KeyFrameSet * frameSet = instance->SourceAnimation->GetKeyFrameSet(frameSetIndex);

	// make sure it's an active KeyFrameSet
	if(frameSet != NULL && frameSet->Used)
	{
		_tempInstance = instance;
		// for each of translation, scale, and rotation, find the two respective key frames between which
		// instance->Progress lies, and interpolate between them based on instance->Progress.
		CalculateInterpolatedTranslation(instance, instance->Progress, *frameSet, translation);
		CalculateInterpolatedScale(instance, instance->Progress, *frameSet, scale);
		CalculateInterpolatedRotation(instance, instance->Progress, *frameSet, rotation);
	}

}

void AnimationPlayer::UpdateAnimationInstanceProgress(AnimationInstanceRef instance) const
{
	// update animation instance progress
	float deltaTime = Time::GetDeltaTime();

	if(instance->Playing && !instance->Paused)
	{
		instance->Progress += deltaTime;

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
 * Use the value of [progress] to find the two closest translation key frames in [keyFrameSet]. Then interpolate between the translation
 * values in those two key frames based on where [progress] lies between them, and store the result in [vector].
 */
void AnimationPlayer::CalculateInterpolatedTranslation(AnimationInstanceRef instance,float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.TranslationKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 1; f < frameCount; f++)
	{
		const TranslationKeyFrame& checkFrame = keyFrameSet.TranslationKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(checkFrame.RealTime > progress || f == frameCount-1)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;
			unsigned int nextIndex = f;
			bool overShoot = false;

			if(f == frameCount-1 && checkFrame.RealTime <= progress)
			{
				lastIndex = f;
				nextIndex = 0;
				overShoot = true;
			}

			const TranslationKeyFrame& nextFrame = keyFrameSet.TranslationKeyFrames[nextIndex];
			const TranslationKeyFrame& lastFrame = keyFrameSet.TranslationKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = nextFrame.RealTime - lastFrame.RealTime;
			if(overShoot)  interFrameTimeDelta = instance->Duration -lastFrame.RealTime;

			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform interpolation
			vector.x = ((nextFrame.Translation.x - lastFrame.Translation.x) * interFrameProgress) + lastFrame.Translation.x;
			vector.y = ((nextFrame.Translation.y - lastFrame.Translation.y) * interFrameProgress) + lastFrame.Translation.y;
			vector.z = ((nextFrame.Translation.z - lastFrame.Translation.z) * interFrameProgress) + lastFrame.Translation.z;

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
void AnimationPlayer::CalculateInterpolatedScale(AnimationInstanceRef instance,float progress, const KeyFrameSet& keyFrameSet, Vector3& vector) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.ScaleKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		const ScaleKeyFrame& checkFrame = keyFrameSet.ScaleKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(checkFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;
			unsigned int nextIndex = f;
			bool overShoot = false;

			if(f == frameCount-1 && checkFrame.RealTime <= progress)
			{
				lastIndex = f;
				nextIndex = 0;
				overShoot = true;
			}

			const ScaleKeyFrame& nextFrame = keyFrameSet.ScaleKeyFrames[nextIndex];
			const ScaleKeyFrame& lastFrame = keyFrameSet.ScaleKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = nextFrame.RealTime - lastFrame.RealTime;
			if(overShoot)  interFrameTimeDelta = instance->Duration -lastFrame.RealTime;

			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform interpolation
			vector.x = ((nextFrame.Scale.x - lastFrame.Scale.x) * interFrameProgress) + lastFrame.Scale.x;
			vector.y = ((nextFrame.Scale.y - lastFrame.Scale.y) * interFrameProgress) + lastFrame.Scale.y;
			vector.z = ((nextFrame.Scale.z - lastFrame.Scale.z) * interFrameProgress) + lastFrame.Scale.z;

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
void AnimationPlayer::CalculateInterpolatedRotation(AnimationInstanceRef instance,float progress, const KeyFrameSet& keyFrameSet, Quaternion& rotation) const
{
	bool foundFrame =false;
	unsigned int frameCount = keyFrameSet.RotationKeyFrames.size();

	// loop through each key frame
	for(unsigned int f = 0; f < frameCount; f++)
	{
		const RotationKeyFrame& checkFrame = keyFrameSet.RotationKeyFrames[f];

		// if the RealTime value for this key frame is greater than [progress], then the last key frame and the current key frame
		// are the frames we want
		if(checkFrame.RealTime > progress  || f == frameCount-1)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;
			unsigned int nextIndex = f;
			bool overShoot = false;

			if(f == frameCount-1 && checkFrame.RealTime <= progress)
			{
				lastIndex = f;
				nextIndex = 0;
				overShoot = true;
			}

			const RotationKeyFrame& nextFrame = keyFrameSet.RotationKeyFrames[nextIndex];
			const RotationKeyFrame& lastFrame = keyFrameSet.RotationKeyFrames[lastIndex];

			// calculate local progress between [lastFrame] and [keyFrame]
			float interFrameTimeDelta = nextFrame.RealTime - lastFrame.RealTime;
			if(overShoot)  interFrameTimeDelta = instance->Duration -lastFrame.RealTime;

			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			// perform spherical interpolation between the two Quaternions
			Quaternion a(lastFrame.Rotation.x(),lastFrame.Rotation.y(),lastFrame.Rotation.z(),lastFrame.Rotation.w());
			Quaternion b(nextFrame.Rotation.x(),nextFrame.Rotation.y(),nextFrame.Rotation.z(),nextFrame.Rotation.w());
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
		weights.push_back(0);

		activeAnimationIndices[animation->GetObjectID()] = animationCount;
		animationCount++;
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
		unsigned int targetIndex = activeAnimationIndices[animation->GetObjectID()];

		float tempLeftOver = 1;
		for(unsigned int i = 0; i < activeAnimations.size(); i++)
		{
			AnimationInstanceRef instance = activeAnimations[i];

			if(i != targetIndex)
			{
				instance->Stop();
				weights[i] = 0;
			}
			else
			{
				weights[i] = 1;
				instance->Play();
			}

			tempLeftOver -= weights[1];
		}
		if(tempLeftOver < 0)tempLeftOver = 0;
		leftOverWeight = tempLeftOver;
		playingAnimationsCount = 1;
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
		unsigned int targetIndex = activeAnimationIndices[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Stop -> invalid animation index found in index map.");

		AnimationInstanceRef instance = activeAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Stop -> Target animation is invalid.");

		if(instance->Playing)playingAnimationsCount--;
		instance->Stop();

		leftOverWeight += weights[targetIndex];
		weights[targetIndex] = 0;
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
		unsigned int targetIndex = activeAnimationIndices[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Pause -> invalid animation index found in index map.");

		AnimationInstanceRef instance = activeAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Pause -> Target animation is invalid.");

		instance->Pause();
	}
}

/*
 * Resume playback of [animation] on this player.
 */
void AnimationPlayer::Resume(AnimationRef animation)
{
	ASSERT_RTRN(animation.IsValid(), "AnimationPlayer::Resume -> Animation is invalid.");
	if(activeAnimationIndices.find(animation->GetObjectID()) != activeAnimationIndices.end())
	{
		unsigned int targetIndex = activeAnimationIndices[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::Resume -> invalid animation index found in index map.");

		AnimationInstanceRef instance = activeAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::Resume -> Target animation is invalid.");

		instance->Play();
	}
}

void AnimationPlayer::CrossFade(AnimationRef animation, float duration)
{
	if(activeAnimationIndices.find(animation->GetObjectID()) != activeAnimationIndices.end())
	{
		unsigned int targetIndex = activeAnimationIndices[animation->GetObjectID()];
		ASSERT_RTRN(targetIndex < animationCount, "AnimationPlayer::CrossFade -> invalid animation index found in index map.");

		AnimationInstanceRef instance = activeAnimations[targetIndex];
		ASSERT_RTRN(instance.IsValid(), "AnimationPlayer::CrossFade -> Target animation is invalid.");

		if(!(instance->Playing))playingAnimationsCount++;
		instance->Reset();
		instance->Play();

		CrossFadeBlendOp * blendOp = new CrossFadeBlendOp(duration, targetIndex);
		ASSERT_RTRN(blendOp, "AnimationPlayer::CrossFade -> Unable to allocate new CrossFadeBlendOp object.");

		bool initSuccess = blendOp->Init(weights);
		if(!initSuccess)
		{
			Debug::PrintError("AnimationPlayer::CrossFade -> Unable to Init new CrossFadeBlendOp object.");
			delete blendOp;
			return;
		}

		blendOp->SetOnCompleteCallback([targetIndex, this](CrossFadeBlendOp * op)
		{
			playingAnimationsCount = 1;

			for(unsigned int i = 0; i < activeAnimations.size(); i++)
			{
				if(i != targetIndex)
				{
					AnimationInstanceRef instance = activeAnimations[i];
					if(!instance.IsValid())
					{
						Debug::PrintWarning("AnimationPlayer::CrossFade::OnCompleteCallback -> Invalid animation found.");
						continue;
					}

					if(instance->Playing)instance->Stop();
				}
			}
		});

		QueueBlendOperation(blendOp);
	}
}
