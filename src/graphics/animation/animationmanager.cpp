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

bool AnimationManager::IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation)
{
	SHARED_REF_CHECK(meshRenderer, "AnimationManager::IsCompatible -> Mesh renderer is not valid.", false);
	SHARED_REF_CHECK(animation, "AnimationManager::IsCompatible -> Animation is not valid.", false);

	SkeletonRef meshSkeleton = meshRenderer->GetSkeleton();
	SkeletonRef animationSkeleton = meshRenderer->GetSkeleton();

	SHARED_REF_CHECK(meshSkeleton, "AnimationManager::IsCompatible -> Mesh renderer does not have a valid skeleton.", false);
	SHARED_REF_CHECK(animationSkeleton, "AnimationManager::IsCompatible -> Animation renderer does not have a valid skeleton.", false);

	unsigned int meshSkeletonNodeCount = meshSkeleton->GetNodeCount();
	if(meshSkeletonNodeCount != animationSkeleton->GetNodeCount())
	{
		std::string msg = std::string("AnimationManager::IsCompatible -> Mismatched node count: ") + std::to_string(meshSkeletonNodeCount);
		msg += std::string(", ") + std::to_string(animationSkeleton->GetNodeCount());
		Debug::PrintError(msg);

		return false;
	}

	for(unsigned int n = 0; n < meshSkeletonNodeCount; n++)
	{
		SkeletonNode * meshNode = meshSkeleton->GetNodeFromList(n);
		SkeletonNode * animationNode = animationSkeleton->GetNodeFromList(n);

		if(meshNode->Name != animationNode->Name)
		{
			Debug::PrintError("AnimationManager::IsCompatible -> Mismatched node names.");
			return false;
		}
	}

	return true;
}


