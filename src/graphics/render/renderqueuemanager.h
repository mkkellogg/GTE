/*
 * class: RenderQueueManager
 *
 * author: Mark Kellogg
 *
 * Manage a set of RenderQueue instances.
 */

#ifndef _GTE_RENDER_QUEUE_MANAGER_H
#define _GTE_RENDER_QUEUE_MANAGER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <stack>
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "renderqueue.h"

namespace GTE
{
	class RenderQueueManager
	{
		public:	

		class Iterator
		{
			friend class RenderQueueManager;

			RenderQueueManager *manager;
			mutable UInt32 queueIndex;
			mutable RenderQueue * currentQueue;
			mutable UInt32 entryIndex;
			mutable Bool valid;
			mutable UInt32 minQueue;
			mutable UInt32 maxQueue;

			Iterator()
			{
				this->manager = manager;
				currentQueue = nullptr;
				queueIndex = 0;
				entryIndex = 0;
				valid = false;
			}
			
			void Init(RenderQueueManager *manager, UInt32 minQueue, UInt32 maxQueue)
			{
				this->manager = manager;
				this->minQueue = minQueue;
				this->maxQueue = maxQueue;
				currentQueue = nullptr;
				queueIndex = 0;
				entryIndex = 0;
				valid = false;

				if(manager != nullptr)
				{
					while(queueIndex < manager->GetRenderQueueCount())
					{
						currentQueue = manager->GetRenderQueueAtIndex(queueIndex);
						if(currentQueue->GetObjectCount() > 0 && currentQueue->GetID() >= minQueue && currentQueue->GetID() <= maxQueue)
						{
							valid = true;
							break;
						}
						queueIndex++;
					}
				}

				if(queueIndex >= manager->GetRenderQueueCount())
				{
					StopIteration();
				}
			}

			void Advance() const
			{
				entryIndex++;
				if(entryIndex >= currentQueue->GetObjectCount())
				{
					queueIndex++;
					while(queueIndex < manager->GetRenderQueueCount())
					{
						currentQueue = manager->GetRenderQueueAtIndex(queueIndex);
						if(currentQueue->GetObjectCount() > 0)
						{
							break;
						}
						queueIndex++;
					}

					if(queueIndex >= manager->GetRenderQueueCount() || currentQueue->GetID() > maxQueue)
					{
						StopIteration();
					}
					else
					{
						entryIndex = 0;
					}
				}
			}

			void StopIteration() const
			{
				currentQueue = nullptr;
				queueIndex = 0;
				entryIndex = 0;
				valid = false;
			}

			public:

			const Iterator& operator ++() const
			{
				if(valid)
				{
					Advance();
				}
				return *this;
			}

			RenderQueueEntry* operator *() const
			{
				if(valid)
					return currentQueue->GetObject(entryIndex);
				else
				{
					return nullptr;
				}
			}

			Bool operator ==(const Iterator& other) const
			{
				return  other.valid == valid &&
						other.queueIndex == queueIndex && 
						other.entryIndex == entryIndex;
			}

			Bool operator !=(const Iterator& other) const
			{
				return  other.valid != valid ||
						other.queueIndex != queueIndex ||
						other.entryIndex != entryIndex;
			}
		};

		typedef const RenderQueueManager::Iterator ConstIterator;

		protected:

		static const UInt32 MAX_RENDER_QUEUES = 128;

		// the render queues that are managed by this instance of RenderQueueManager
		RenderQueue* renderQueues[MAX_RENDER_QUEUES];

		UInt32 renderQueueCount;
		UInt32 minQueue;
		UInt32 maxQueue;

		Iterator theIterator;
		Iterator endIterator;

		public:
		
		RenderQueueManager();
		virtual ~RenderQueueManager();

		RenderQueue* GetRenderQueueForID(UInt32 id);
		RenderQueue* GetRenderQueueAtIndex(UInt32 index);
		void ClearAllRenderQueues();
		void DestroyRenderQueues();

		UInt32 GetRenderQueueCount() const;

		ConstIterator& Begin();
		ConstIterator& Begin(Bool limitQueue, UInt32 minQueue, UInt32 maxQueue);
		ConstIterator& BeginWithRange(UInt32 minQueue, UInt32 maxQueue);
		ConstIterator& End();
	};
}

#endif
