#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationmanager.h"
#include "engine.h"
#include "object/enginetypes.h"
#include "object/engineobject.h"
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
 * Check if the Skeleton specified by [skeleton] is compatible with [animation]. This simply
 * means loop through each channel in [animation] and verify there is a corresponding node in
 * [skeleton. This match-up is accomplished by matching the name of the channel to the name
 * of the skeleton node.
 */
bool AnimationManager::IsCompatible(SkeletonRef skeleton, AnimationRef animation) const
{
	ASSERT(skeleton.IsValid(), "AnimationManager::IsCompatible -> Skeleton is not valid.", false);
	ASSERT(animation.IsValid(), "AnimationManager::IsCompatible -> Animation is not valid.", false);

	unsigned int skeletonNodeCount = skeleton->GetNodeCount();
	unsigned int channelCount = animation->GetChannelCount();

	// verify matching node count
	if(skeletonNodeCount != channelCount)
	{
		//std::string msg = std::string("AnimationManager::IsCompatible -> Mismatched node count: ") + std::to_string(skeletonNodeCount);
		//msg += std::string(", ") + std::to_string(channelCount);
		//Debug::PrintWarning(msg);
		//return false;
	}

	// verify each channel in the animation has a matching node in [skeleton]
	for(unsigned int c = 0; c < channelCount; c++)
	{
		bool foundNodeForChannel = false;
		const std::string * channelName = animation->GetChannelName(c);
		if(channelName == NULL)continue;

		for(unsigned int n = 0; n < skeletonNodeCount; n++)
		{
			SkeletonNode * node = skeleton->GetNodeFromList(n);
			if(node->Name == *channelName)
			{
				foundNodeForChannel = true;
				break;
			}
		}

		if(!foundNodeForChannel)
		{
			//std::string msg("AnimationManager::IsCompatible -> Could not find matching node for: ");
			//msg += *channelName;
			//Debug::PrintWarning(msg);
			//return false;
		}
	}

	return true;
}

/*
 * Determine if the Skeleton object belonging to [meshRenderer] is compatible with target Skeleton of [animation].
 */
bool AnimationManager::IsCompatible(SkinnedMesh3DRendererRef meshRenderer, AnimationRef animation) const
{
	ASSERT(meshRenderer.IsValid(), "AnimationManager::IsCompatible -> Mesh renderer is not valid.", false);
	ASSERT(meshRenderer->GetSkeleton().IsValid(), "AnimationManager::IsCompatible -> Mesh skeleton is not valid.", false);

	return IsCompatible(meshRenderer->GetSkeleton(), animation);
}

/*
 * Loop through each active AnimationPlayer and drive its playback.
 */
void AnimationManager::Update()
{
	for(std::unordered_map<unsigned int, AnimationPlayerRef>::iterator iter = activePlayers.begin(); iter != activePlayers.end(); ++iter)
	{
		AnimationPlayerRef player = iter->second;

		if(player.IsValid())
		{
			player->Update();
		}
	}
}

/*
 * Check active players to see if any are playing animations for [target]. If not, create one
 * and assign it to [target].
 */
AnimationPlayerRef AnimationManager::RetrieveOrCreateAnimationPlayer(SkeletonRef target)
{
	ASSERT(target.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Target is not valid.", AnimationPlayerRef::Null());

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	ASSERT(objectManager != NULL, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Engine object manager is NULL.", AnimationPlayerRef::Null());

	if(activePlayers.find(target->GetObjectID()) == activePlayers.end())
	{
		AnimationPlayerRef player = objectManager->CreateAnimationPlayer(target);
		ASSERT(player.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Unable to create player.", AnimationPlayerRef::Null());

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
	ASSERT(renderer.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Mesh renderer is not valid.", AnimationPlayerRef::Null());
	return RetrieveOrCreateAnimationPlayer(renderer->GetSkeleton());
}





