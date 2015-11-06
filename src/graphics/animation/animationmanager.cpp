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
#include "global/assert.h"
#include "global/constants.h"
#include "util/time.h"
#include "debug/gtedebug.h"
#include <string>
#include <unordered_map>

namespace GTE
{
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
	Bool AnimationManager::IsCompatible(SkeletonSharedConstPtr skeleton, AnimationSharedConstPtr animation) const
	{
		NONFATAL_ASSERT_RTRN(skeleton.IsValid(), "AnimationManager::IsCompatible -> Skeleton is not valid.", false, true);
		NONFATAL_ASSERT_RTRN(animation.IsValid(), "AnimationManager::IsCompatible -> Animation is not valid.", false, true);

		UInt32 skeletonNodeCount = skeleton->GetNodeCount();
		UInt32 channelCount = animation->GetChannelCount();

		// verify matching node count
		if (skeletonNodeCount != channelCount)
		{
			//std::string msg = std::string("AnimationManager::IsCompatible -> Mismatched node count: ") + std::to_string(skeletonNodeCount);
			//msg += std::string(", ") + std::to_string(channelCount);
			//Debug::PrintWarning(msg);
			//return false;
		}

		// verify each channel in the animation has a matching node in [skeleton]
		for (UInt32 c = 0; c < channelCount; c++)
		{
			Bool foundNodeForChannel = false;
			const std::string * channelName = animation->GetChannelName(c);
			if (channelName == nullptr)continue;

			for (UInt32 n = 0; n < skeletonNodeCount; n++)
			{
				Skeleton * skeletonPtr = const_cast<Skeleton *>(skeleton.GetConstPtr());
				SkeletonNode * node = skeletonPtr->GetNodeFromList(n);
				if (node->Name == *channelName)
				{
					foundNodeForChannel = true;
					break;
				}
			}

			if (!foundNodeForChannel)
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
	Bool AnimationManager::IsCompatible(SkinnedMesh3DRendererSharedConstPtr meshRenderer, AnimationSharedConstPtr animation) const
	{
		NONFATAL_ASSERT_RTRN(meshRenderer.IsValid(), "AnimationManager::IsCompatible -> Mesh renderer is not valid.", false, true);

		SkinnedMesh3DRenderer * rendererPtr = const_cast<SkinnedMesh3DRenderer *>(meshRenderer.GetConstPtr());
		NONFATAL_ASSERT_RTRN(rendererPtr->GetSkeleton().IsValid(), "AnimationManager::IsCompatible -> Mesh skeleton is not valid.", false, true);

		return IsCompatible(rendererPtr->GetSkeleton(), animation);
	}

	/*
	 * Loop through each active AnimationPlayer and drive its playback.
	 */
	void AnimationManager::Update()
	{
		for (std::unordered_map<UInt32, AnimationPlayerSharedPtr>::iterator iter = activePlayers.begin(); iter != activePlayers.end(); ++iter)
		{
			AnimationPlayerSharedPtr player = iter->second;

			if (player.IsValid())
			{
				player->Update();
			}
		}
	}

	/*
	 * Check active players to see if any are playing animations for [target]. If not, create one
	 * and assign it to [target].
	 */
	AnimationPlayerSharedPtr AnimationManager::RetrieveOrCreateAnimationPlayer(SkeletonSharedPtr target)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Target is not valid.", AnimationPlayerSharedPtr::Null(), true);

		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
		ASSERT(objectManager != nullptr, "AnimationManager::RetrieveOrCreateAnimationPlayer -> Engine object manager is null.");

		if (activePlayers.find(target->GetObjectID()) == activePlayers.end())
		{
			AnimationPlayerSharedPtr player = objectManager->CreateAnimationPlayer(target);
			NONFATAL_ASSERT_RTRN(player.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Unable to create player.", AnimationPlayerSharedPtr::Null(), false);

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
	AnimationPlayerSharedPtr AnimationManager::RetrieveOrCreateAnimationPlayer(SkinnedMesh3DRendererSharedConstPtr renderer)
	{
		NONFATAL_ASSERT_RTRN(renderer.IsValid(), "AnimationManager::RetrieveOrCreateAnimationPlayer -> Mesh renderer is not valid.", AnimationPlayerSharedPtr::Null(), true);

		SkinnedMesh3DRenderer * rendererPtr = const_cast<SkinnedMesh3DRenderer *>(renderer.GetConstPtr());
		return RetrieveOrCreateAnimationPlayer(rendererPtr->GetSkeleton());
	}
}





