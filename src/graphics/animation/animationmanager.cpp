#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationmanager.h"
#include "object/enginetypes.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/skeleton.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{

}

bool AnimationManager::IsCompatible(SkeletonRef skeleton, AnimationRef animation)
{
	SHARED_REF_CHECK(skeleton, "AnimationManager::IsCompatible -> Skeleton is not valid.", false);
	SHARED_REF_CHECK(animation, "AnimationManager::IsCompatible -> Animation is not valid.", false);

	SkeletonRef animationSkeleton = animation->GetSkeleton();

	SHARED_REF_CHECK(animationSkeleton, "AnimationManager::IsCompatible -> Animation does not have a valid skeleton.", false);

	unsigned int skeletonNodeCount = skeleton->GetNodeCount();
	for(unsigned int n = 0; n < skeletonNodeCount && n < animationSkeleton->GetNodeCount(); n++)
	{
		SkeletonNode * meshNode = skeleton->GetNodeFromList(n);
		SkeletonNode * animationNode = animationSkeleton->GetNodeFromList(n);

		printf("%s <==> %s\n", meshNode->Name.c_str(), animationNode->Name.c_str());
	}

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


