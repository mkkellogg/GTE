#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <vector>
#include "renderqueuemanager.h"
#include "engine.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

namespace GTE
{
	RenderQueueManager::RenderQueueManager()
	{
		renderQueueCount = 0;
	}

	RenderQueueManager::~RenderQueueManager()
	{
		DestroyRenderQueues();
	}

	/*
	* Empty out all render queues.
	*/
	void RenderQueueManager::ClearAllRenderQueues()
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT(queue != nullptr, "RenderQueueManager::ClearAllRenderQueues -> Null render queue encountered.", true);

			queue->Clear();
		}
	}

	/*
	* Return the render queue that is linked to [renderQueueID]. If it doesn't
	* yet exist, create it.
	*/
	RenderQueue* RenderQueueManager::GetRenderQueueForID(UInt32 renderQueueID)
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT_RTRN(queue != nullptr, "RenderQueueManager::GetRenderQueue -> Null render queue encountered.", nullptr, true);

			if(queue->GetID() == renderQueueID)
			{
				return queue;
			}
		}
		ASSERT(renderQueueCount < MAX_RENDER_QUEUES, "RenderQueueManager::GetRenderQueue -> Maximum number of render queues exceeded!");

		// create new queue for [renderQueueID], since it doesn't yet exist
		RenderQueue * newQueue = new(std::nothrow) RenderQueue(renderQueueID, 128, 128);
		ASSERT(newQueue != nullptr, "RenderQueueManager::GetRenderQueue -> Unable to allocate to render queue.");
		renderQueues[renderQueueCount] = newQueue;
		renderQueueCount++;

		//TODO: sort more efficiently (quick sort would be best since it's in-place)
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			for(UInt32 j = 0; j < renderQueueCount; j++)
			{
				if(j < renderQueueCount - 1 && renderQueues[j] > renderQueues[j + 1])
				{
					RenderQueue * temp = renderQueues[j];
					renderQueues[j] = renderQueues[j + 1];
					renderQueues[j + 1] = temp;
				}
			}
		}

		return newQueue;
	}

	/*
	* Return the render queue that is at [index] in [renderQueues]
	*/
	RenderQueue* RenderQueueManager::GetRenderQueueAtIndex(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < renderQueueCount, "RenderQueueManager::GetRenderQueueAtIndex -> 'index' is out of range.", nullptr, true);
		return renderQueues[index];
	}

	/*
	* Deallocate & destroy all render queues.
	*/
	void RenderQueueManager::DestroyRenderQueues()
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			if(renderQueues[i] != nullptr)
			{
				delete renderQueues[i];
				renderQueues[i] = nullptr;
			}
		}
	}

	/*
	* Return the number of RenderQueue instances being managed.
	*/
	UInt32 RenderQueueManager::GetRenderQueueCount() const
	{
		return renderQueueCount;
	}

	RenderQueueManager::ConstIterator& RenderQueueManager::Begin()
	{
		theIterator.Init(this);
		return theIterator;
	}

	RenderQueueManager::ConstIterator& RenderQueueManager::End()
	{
		endIterator.valid = false;
		return endIterator;
	}
}