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
#include "graphics/animation/animationplayer.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/bone.h"
#include "global/global.h"
#include "global/constants.h"
#include "util/time.h"
#include "ui/debug.h"
#include <string>
#include <unordered_map>

// singleton instance
AnimationManager * AnimationManager::instance = NULL;

/*
 * Default constructor
 */
AnimationManager::AnimationManager()
{

}

/*
 * Destructor
 */
AnimationManager::~AnimationManager()
{

}

/*
 * Accessor for the singleton instance
 */
AnimationManager * AnimationManager::Instance()
{
	// TODO: implement double checked locking
	if(instance == NULL)
	{
		instance = new AnimationManager();
	}

	return instance;
}

/*
 * Check if the Skeleton specified by [skeleton] is compatible with [animation]. This simply means check
 * the target Skeleton of [animation] to see if it has the same number of nodes and that those nodes
 * are ordered the same. The latter is verified by running through the node list by index and making
 * sure the node names match.
 *
 */
bool AnimationManager::IsCompatible(SkeletonRef skeleton, AnimationRef animation) const
{
	SHARED_REF_CHECK(skeleton, "AnimationManager::IsCompatible -> Skeleton is not valid.", false);
	SHARED_REF_CHECK(animation, "AnimationManager::IsCompatible -> Animation is not valid.", false);

	SkeletonRef animationSkeleton = animation->GetTarget();

	SHARED_REF_CHECK(animationSkeleton, "AnimationManager::IsCompatible -> Animation does not have a valid skeleton.", false);

	unsigned int skeletonNodeCount = skeleton->GetNodeCount();

	// verify matching node count
	if(skeletonNodeCount != animationSkeleton->GetNodeCount())
	{
		std::string msg = std::string("AnimationManager::IsCompatible -> Mismatched node count: ") + std::to_string(skeletonNodeCount);
		msg += std::string(", ") + std::to_string(animationSkeleton->GetNodeCount());
		Debug::PrintWarning(msg);
		return false;
	}

	// verify the name of each node matches the name of the node at the same index in
	// the other Skeleton
	for(unsigned int n = 0; n < skeletonNodeCount; n++)
	{
		SkeletonNode * meshNode = skeleton->GetNodeFromList(n);
		SkeletonNode * animationNode = animationSkeleton->GetNodeFromList(n);

		if(meshNode->Name != animationNode->Name)
		{
			Debug::PrintWarning("AnimationManager::IsCompatible -> Mismatched node names.");
			return false;
		}
	}

	return true;
}

/*
 * Determine if the Skeleton object belonging to [meshRenderer] is compatible with target Skeleton of [animation].
 */
bool AnimationManager::IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation) const
{
	SHARED_REF_CHECK(meshRenderer, "AnimationManager::IsCompatible -> Mesh renderer is not valid.", false);
	SHARED_REF_CHECK(meshRenderer->GetSkeleton(), "AnimationManager::IsCompatible -> Mesh skeleton is not valid.", false);

	return IsCompatible(meshRenderer->GetSkeleton(), animation);
}

/*
 * Loop through each active AnimationPlayer and drive its playback.
 */
void AnimationManager::Drive()
{
	for(std::unordered_map<unsigned int, AnimationPlayerRef>::iterator iter = activePlayers.begin(); iter != activePlayers.end(); ++iter)
	{
		AnimationPlayerRef player = iter->second;

		if(player.IsValid())
		{
			player->Drive();
		}
	}
}

/*
 * Check active players to see if any are playing animations for [target]. If not, create one
 * and assign it to [target].
 */
AnimationPlayerRef AnimationManager::RetrieveOrCreateAnimationPlayer(SkeletonRef target)
{
	SHARED_REF_CHECK(target, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Target is not valid.", AnimationPlayerRef::Null());

	EngineObjectManager * objectManager = EngineObjectManager::Instance();
	NULL_CHECK(objectManager, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Engine object manager is NULL.", AnimationPlayerRef::Null());

	if(activePlayers.find(target->GetObjectID()) == activePlayers.end())
	{
		AnimationPlayerRef player = objectManager->CreateAnimationPlayer(target);
		SHARED_REF_CHECK(player, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Unable to create player.", AnimationPlayerRef::Null());

		// put the newly created AnimationPlayer in the list of active players.s
		activePlayers[target->GetObjectID()] = player;
		return player;
	}

	return activePlayers[target->GetObjectID()];
}

/*
 * Check active players to see if any are playing animations for the skeleton belonging to [renderer].
 * If not, create one and assign it to [target].
 */
AnimationPlayerRef AnimationManager::RetrieveOrCreateAnimationPlayer(SkinnedMesh3DRendererRef renderer)
{
	SHARED_REF_CHECK(renderer, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Mesh renderer is not valid.", AnimationPlayerRef::Null());
	return RetrieveOrCreateAnimationPlayer(renderer->GetSkeleton());
}





