#include "renderqueue.h"
#include "base/binarymask.h"
#include "global/global.h"
#include "global/assert.h"
#include "geometry/transform.h"

namespace GTE
{

	RenderQueue::RenderQueue(UInt32 id, UInt32 initialCount, UInt32 increaseCount)
	{
		this->id = id;
		this->increaseCount = increaseCount;
		renderObjects = nullptr;
		totalCount = 0;
		realCount = 0;

		IncreaseCount(initialCount);
	}

	RenderQueue::~RenderQueue()
	{
		SAFE_DELETE_ARRAY(renderObjects);
	}

	UInt32 RenderQueue::GetID()
	{
		return id;
	}

	void RenderQueue::Clear()
	{
		realCount = 0;
	}

	void RenderQueue::IncreaseCount(UInt32 count)
	{
		RenderQueueEntry * temp = new (std::nothrow) RenderQueueEntry[totalCount + count];
		ASSERT(temp != nullptr, "RenderQueue::IncreaseCount -> Unable to allocate render queue entries.");

		if(renderObjects == nullptr)
		{
			renderObjects = temp;
		}
		else
		{
			for(UInt32 i = 0; i < realCount; i++)
			{
				temp[i] = renderObjects[i];
			}
			delete[] renderObjects;
			renderObjects = temp;
		}

		totalCount += count;
	}

	void RenderQueue::Add(SceneObject* container, SubMesh3D* mesh, SubMesh3DRenderer* renderer, MaterialSharedPtr* renderMaterial, Mesh3DFilter* meshFilter, Transform* aggregateTransform)
	{
		if(realCount >= totalCount)
		{
			IncreaseCount(64);
		}

		RenderQueueEntry& entry = renderObjects[realCount];
		entry.Container = container;
		entry.Mesh = mesh;
		entry.Renderer = renderer;
		entry.RenderMaterial = renderMaterial;
		entry.MeshFilter = meshFilter;
		realCount++;
	}

	RenderQueueEntry* RenderQueue::GetObject(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < realCount, "RenderQueue::GetObject -> 'index' is out of range.", nullptr, true);
		return renderObjects + index;
	}

	UInt32 RenderQueue::GetObjectCount()
	{
		return realCount;
	}
}

