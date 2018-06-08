#include "renderqueuemanager.h"
#include "engine.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

namespace GTE {
    RenderQueueManager::RenderQueueManager() {
        renderQueueCount = 0;
        minQueue = 0;
        maxQueue = 0;
    }

    RenderQueueManager::~RenderQueueManager() {
        DestroyRenderQueues();
    }

    /*
    * Empty out all render queues.
    */
    void RenderQueueManager::ClearAllRenderQueues() {
        for (UInt32 i = 0; i < renderQueueCount; i++) {
            RenderQueue* queue = renderQueues[i];
            NONFATAL_ASSERT(queue != nullptr, "RenderQueueManager::ClearAllRenderQueues -> Null render queue encountered.", true);

            queue->Clear();
        }
    }

    /*
    * Get the queue ID of the render queue at [index].
    */
    Int32 RenderQueueManager::GetRenderQueueID(UInt32 index) {
        NONFATAL_ASSERT_RTRN(index < renderQueueCount, "RenderQueueManager::GetRenderQueueID -> Index is out of range.", -1, true);

        return renderQueues[index]->GetID();
    }

    /*
    * Return the render queue that is linked to [renderQueueID]. If it doesn't
    * yet exist, create it.
    */
    RenderQueue* RenderQueueManager::GetRenderQueueForID(UInt32 renderQueueID) {
        if (renderQueueCount == 0) {
            minQueue = renderQueueID;
            maxQueue = renderQueueID;
        }
        else {
            if (renderQueueID < minQueue)minQueue = renderQueueID;
            if (renderQueueID > maxQueue)maxQueue = renderQueueID;
        }

        for (UInt32 i = 0; i < renderQueueCount; i++) {
            RenderQueue* queue = renderQueues[i];
            NONFATAL_ASSERT_RTRN(queue != nullptr, "RenderQueueManager::GetRenderQueue -> Null render queue encountered.", nullptr, true);

            if (queue->GetID() == renderQueueID) {
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
        for (UInt32 i = 0; i < renderQueueCount; i++) {
            for (UInt32 j = 0; j < renderQueueCount; j++) {
                if (j < renderQueueCount - 1 && renderQueues[j]->GetID() > renderQueues[j + 1]->GetID()) {
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
    RenderQueue* RenderQueueManager::GetRenderQueueAtIndex(UInt32 index) {
        NONFATAL_ASSERT_RTRN(index < renderQueueCount, "RenderQueueManager::GetRenderQueueAtIndex -> 'index' is out of range.", nullptr, true);
        return renderQueues[index];
    }

    /*
    * Deallocate & destroy all render queues.
    */
    void RenderQueueManager::DestroyRenderQueues() {
        for (UInt32 i = 0; i < renderQueueCount; i++) {
            if (renderQueues[i] != nullptr) {
                delete renderQueues[i];
                renderQueues[i] = nullptr;
            }
        }
    }

    /*
    * Return the number of RenderQueue instances being managed.
    */
    UInt32 RenderQueueManager::GetRenderQueueCount() const {
        return renderQueueCount;
    }

    /*
    * Get the largest queue ID managed by this render queue manager;
    */
    UInt32 RenderQueueManager::GetMaxQueue() const {
        return maxQueue;
    }

    /*
    * Get the smallest queue ID managed by this render queue manager;
    */
    UInt32 RenderQueueManager::GetMinQueue() const {
        return minQueue;
    }

    RenderQueueManager::ConstIterator RenderQueueManager::Begin() {
        return BeginWithRange(this->minQueue, this->maxQueue);
    }

    RenderQueueManager::ConstIterator RenderQueueManager::Begin(UInt32 minQueue, UInt32 maxQueue) {
        return BeginWithRange(minQueue, maxQueue);
    }

    RenderQueueManager::ConstIterator RenderQueueManager::BeginWithRange(UInt32 minQueue, UInt32 maxQueue) {
        Iterator begin;
        begin.Init(this, minQueue, maxQueue);
        return begin;
    }

    RenderQueueManager::ConstIterator RenderQueueManager::End() {
        return Iterator();
    }
}
