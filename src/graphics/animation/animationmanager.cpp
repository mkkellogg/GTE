#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationmanager.h"
#include "object/enginetypes.h"
#include "object/engineobjectmanager.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/bone.h"
#include "global/global.h"
#include "global/constants.h"
#include "util/time.h"
#include "ui/debug.h"
#include <string>
#include <unordered_map>

AnimationManager * AnimationManager::instance = NULL;

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}

AnimationManager * AnimationManager::Instance()
{
	// TODO: implement double checked locking
	if(instance == NULL)
	{
		instance = new AnimationManager();
	}

	return instance;
}

bool AnimationManager::IsCompatible(SkeletonRef skeleton, AnimationRef animation)
{
	SHARED_REF_CHECK(skeleton, "AnimationManager::IsCompatible -> Skeleton is not valid.", false);
	SHARED_REF_CHECK(animation, "AnimationManager::IsCompatible -> Animation is not valid.", false);

	SkeletonRef animationSkeleton = animation->GetSkeleton();

	SHARED_REF_CHECK(animationSkeleton, "AnimationManager::IsCompatible -> Animation does not have a valid skeleton.", false);

	unsigned int skeletonNodeCount = skeleton->GetNodeCount();
	/*for(unsigned int n = 0; n < skeletonNodeCount && n < animationSkeleton->GetNodeCount(); n++)
	{
		SkeletonNode * meshNode = skeleton->GetNodeFromList(n);
		SkeletonNode * animationNode = animationSkeleton->GetNodeFromList(n);

		printf("%s <==> %s\n", meshNode->Name.c_str(), animationNode->Name.c_str());
	}*/

	if(skeletonNodeCount != animationSkeleton->GetNodeCount())
	{
		std::string msg = std::string("AnimationManager::IsCompatible -> Mismatched node count: ") + std::to_string(skeletonNodeCount);
		msg += std::string(", ") + std::to_string(animationSkeleton->GetNodeCount());
		Debug::PrintError(msg);

		return false;
	}

	for(unsigned int n = 0; n < skeletonNodeCount; n++)
	{
		SkeletonNode * meshNode = skeleton->GetNodeFromList(n);
		SkeletonNode * animationNode = animationSkeleton->GetNodeFromList(n);

		if(meshNode->Name != animationNode->Name)
		{
			Debug::PrintError("AnimationManager::IsCompatible -> Mismatched node names.");
			return false;
		}
	}

	return true;
}

bool AnimationManager::IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation)
{
	SHARED_REF_CHECK(meshRenderer, "AnimationManager::IsCompatible -> Mesh renderer is not valid.", false);
	SHARED_REF_CHECK(meshRenderer->GetSkeleton(), "AnimationManager::IsCompatible -> Mesh skeleton is not valid.", false);

	return IsCompatible(meshRenderer->GetSkeleton(), animation);
}

void AnimationManager::Drive()
{
	for(std::unordered_map<unsigned int, std::vector<AnimationInstanceRef>>::iterator iter = activeAnimations.begin(); iter != activeAnimations.end(); ++iter)
	{
		unsigned int objectID = iter->first;
		std::vector<AnimationInstanceRef>& animations = iter->second;

		if(animations.size() > 0)
		{
			// TODO: Implement animation blending.
			// For now simply use the first animation in the list and apply with weight of 1.
			AnimationInstanceRef instance = animations[0];
			UpdateAnimationInstance(instance);
		}
	}
}

AnimationInstanceRef AnimationManager::CreateAnimationInstance(SkeletonRef skeleton, AnimationRef animation)
{
	if(IsCompatible(skeleton,animation))
	{
		EngineObjectManager * objectManager = EngineObjectManager::Instance();
		AnimationInstanceRef instance = objectManager->CreateAnimationInstance(skeleton, animation);

		SHARED_REF_CHECK(instance, " AnimationManager::CreateAnimationInstance -> Unable to create animation instance.", AnimationInstanceRef::Null());

		bool initSuccess = instance->Init();
		if(!initSuccess)
		{
			Debug::PrintError("AnimationManager::CreateAnimationInstance -> Unable to initialize animation instance.");
			return AnimationInstanceRef::Null();
		}

		unsigned int objectID = skeleton->GetObjectID();

		std::vector<AnimationInstanceRef>& animList = activeAnimations[objectID];

		animList.push_back(instance);

		return instance;
	}
	else
	{
		Debug::PrintError("AnimationManager::CreateAnimationInstance -> Skeleton is not compatible with animation.");
		return AnimationInstanceRef::Null();
	}
}

AnimationInstanceRef AnimationManager::CreateAnimationInstance(SkinnedMesh3DRendererRef renderer, AnimationRef animation)
{
	SHARED_REF_CHECK(renderer, " AnimationManager::CreateAnimationInstance -> Invalid renderer.", AnimationInstanceRef::Null());

	return CreateAnimationInstance(renderer->GetSkeleton(), animation);
}

void AnimationManager::UpdateAnimationInstance(AnimationInstanceRef instance)
{
	float deltaTime = Time::GetDeltaTime();

	SHARED_REF_CHECK_RTRN(instance, " AnimationManager::UpdateAnimationInstance -> Animation instance is invalid.");

	if(instance->IsPlaying())
	{
		for(unsigned int state = 0; state < instance->StateCount; state++)
		{
			AnimationInstance::FrameState * frameState = instance->GetFrameState(state);

			unsigned int frameSetIndex = state;
			KeyFrameSet * frameSet = instance->Animation->GetKeyFrameSet(frameSetIndex);
			if(frameSet != NULL && frameSet->Used)
			{
				Matrix4x4 rotMatrix;
				Matrix4x4 matrix;

				Vector3 translation;
				CalculateInterpolatedTranslation(instance->Progress,instance->Duration, *frameSet, translation);

				Vector3 scale;
				CalculateInterpolatedScale(instance->Progress,instance->Duration, *frameSet, scale);

				Quaternion rotation;
				CalculateInterpolatedRotation(instance->Progress,instance->Duration, *frameSet, rotation);

				Matrix4x4::SetRotate(&rotMatrix, rotation.x(), rotation.y(), rotation.z(), rotation.w()/Constants::TwoPI * 360);

				aiQuaternion rotQuat;
				rotQuat.x = rotation.x();
				rotQuat.y = rotation.y();
				rotQuat.z = rotation.z();
				rotQuat.w = rotation.w();
				aiMatrix3x3t<float> mat = rotQuat.GetMatrix();
				float matData[16];
				matData[0] = mat.a1;
				matData[1] = mat.b1;
				matData[2] = mat.c1;
				matData[3] = 0;
				matData[4] = mat.a2;
				matData[5] = mat.b2;
				matData[6] = mat.c2;
				matData[7] = 0;
				matData[8] = mat.a3;
				matData[9] = mat.b3;
				matData[10] = mat.c3;
				matData[11] = 0;
				matData[12] = 0;
				matData[13] = 0;
				matData[14] = 0;
				matData[15] = 1;

				/*if(frameSetIndex == 21)
				{
					printf("\n");
					printf("\n");
					printf("-- assimp quat -- \n");
					for(unsigned int i =0; i < 16; i++)
					{
						printf("%f,", matData[i]);
					}
					printf("\n");
					printf("\n");
					printf("-- Matrix4x4::SetRotate -- \n");
					for(unsigned int i =0; i < 16; i++)
					{
						printf("%f,", rotMatrix.GetDataPtr()[i]);
					}
					printf("\n");
				}*/

				rotMatrix.SetTo(matData);

				matrix.Scale(scale.x,scale.y,scale.z);
				matrix.PreMultiply(&rotMatrix);
				matrix.PreTranslate(translation.x, translation.y, translation.z);

				SkeletonNode * targetNode = instance->Target->GetNodeFromList(frameSetIndex);
				if(targetNode->HasTarget())
				{
					Transform * localTransform = targetNode->GetLocalTransform();
					if(localTransform != NULL)
					{
						localTransform->SetTo(&matrix);
					}
				}

				if(frameSetIndex == 21)
				{
					//printf("%s, w: %f\n",targetNode->Name.c_str(), rotation.w());
				}
				//printf("index: %d\n", frameSetIndex);
			}
		}

		//printf("prog: %f, dur: %f\n", instance->Progress, instance->Duration);
		instance->Progress += deltaTime;

		//TODO: update to either stop or loop based on settings. for now auto-loop.
		if(instance->Progress >= instance->Duration)
		{
			instance->Progress = instance->Progress - instance->Duration;
			if(instance->Progress < 0) instance->Progress = 0;
		}

		instance->ProgressTicks = instance->Progress * instance->Animation->GetTicksPerSecond();
	}
}

void AnimationManager::CalculateInterpolatedTranslation(float progress, float duration, KeyFrameSet& keyFrameSet, Vector3& vector)
{
	for(unsigned int f = 0; f < keyFrameSet.TranslationKeyFrames.size(); f++)
	{
		TranslationKeyFrame& keyFrame = keyFrameSet.TranslationKeyFrames[f];
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			TranslationKeyFrame& lastFrame = keyFrameSet.TranslationKeyFrames[lastIndex];

			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			vector.x = ((keyFrame.Translation.x - lastFrame.Translation.x) * interFrameProgress) + lastFrame.Translation.x;
			vector.y = ((keyFrame.Translation.y - lastFrame.Translation.y) * interFrameProgress) + lastFrame.Translation.y;
			vector.z = ((keyFrame.Translation.z - lastFrame.Translation.z) * interFrameProgress) + lastFrame.Translation.z;

			//printf("x: %f\n",vector.x);


			break;
		}
	}
}

void AnimationManager::CalculateInterpolatedScale(float progress, float duration, KeyFrameSet& keyFrameSet, Vector3& vector)
{
	for(unsigned int f = 0; f < keyFrameSet.ScaleKeyFrames.size(); f++)
	{
		ScaleKeyFrame& keyFrame = keyFrameSet.ScaleKeyFrames[f];
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			ScaleKeyFrame& lastFrame = keyFrameSet.ScaleKeyFrames[lastIndex];

			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;

			vector.x = ((keyFrame.Scale.x - lastFrame.Scale.x) * interFrameProgress) + lastFrame.Scale.x;
			vector.y = ((keyFrame.Scale.y - lastFrame.Scale.y) * interFrameProgress) + lastFrame.Scale.y;
			vector.z = ((keyFrame.Scale.z - lastFrame.Scale.z) * interFrameProgress) + lastFrame.Scale.z;


			break;
		}
	}
}

void AnimationManager::CalculateInterpolatedRotation(float progress, float duration, KeyFrameSet& keyFrameSet, Quaternion& rotation)
{
	for(unsigned int f = 0; f < keyFrameSet.RotationKeyFrames.size(); f++)
	{
		RotationKeyFrame& keyFrame = keyFrameSet.RotationKeyFrames[f];
		if(keyFrame.RealTime > progress)
		{
			unsigned int lastIndex = 0;
			if(f > 0)lastIndex = f-1;

			RotationKeyFrame& lastFrame = keyFrameSet.RotationKeyFrames[lastIndex];

			float interFrameTimeDelta = keyFrame.RealTime - lastFrame.RealTime;
			float interFrameElapsed = progress - lastFrame.RealTime;
			float interFrameProgress = 1;
			if(interFrameTimeDelta >0)interFrameProgress = interFrameElapsed/interFrameTimeDelta;


			//float x = ((keyFrame.Rotation.x() - lastFrame.Rotation.x()) * interFrameProgress) + lastFrame.Rotation.x();
			//float y = ((keyFrame.Rotation.y() - lastFrame.Rotation.y()) * interFrameProgress) + lastFrame.Rotation.y();
			//float z = ((keyFrame.Rotation.z() - lastFrame.Rotation.z()) * interFrameProgress) + lastFrame.Rotation.z();
			//float w = ((keyFrame.Rotation.w() - lastFrame.Rotation.w()) * interFrameProgress) + lastFrame.Rotation.w();

			aiQuaternion start;
			aiQuaternion end;
			aiQuaternion out;

			start.x = lastFrame.Rotation.x();
			start.y = lastFrame.Rotation.y();
			start.z = lastFrame.Rotation.z();
			start.w = lastFrame.Rotation.w();

			end.x = keyFrame.Rotation.x();
			end.y = keyFrame.Rotation.y();
			end.z = keyFrame.Rotation.z();
			end.w = keyFrame.Rotation.w();

			aiQuaternion::Interpolate(out, start, end, interFrameProgress);

			//rotation.Set(x,y,z,w);
			rotation.Set(out.x,out.y,out.z,out.w);

			//printf("w: %f\n",rotation.w());
			//printf("index: %d\n", lastIndex);

			break;
		}
	}
}



