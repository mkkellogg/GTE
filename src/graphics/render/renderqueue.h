/*
 * class:  RenderQueue
 *
 * Author: Mark Kellogg
 *
 * Container for groups of renderable objects that can be used to
 * organize them into a specific rendering order.
 *
 */

#ifndef _GTE_RENDER_QUEUE_H_
#define _GTE_RENDER_QUEUE_H_

#include "base/intmask.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include <vector>

namespace GTE
{
	// forward declarations
	class SubMesh3D;
	class SceneObject;
	class Transform;

	class RenderQueueEntry
	{
		public:

		RenderQueueEntry()
		{
			Container = SceneObjectRef::Null();;
			Mesh = SubMesh3DRef::Null();
			Renderer = SubMesh3DRendererRef::Null();
			RenderMaterial = MaterialRef::Null();
			MeshFilter = Mesh3DFilterRef::Null();
			AggregateTransform = nullptr;
		}

		RenderQueueEntry(SceneObjectRef container, SubMesh3DRef mesh, SubMesh3DRendererRef renderer, MaterialRef renderMaterial, Mesh3DFilterRef meshFilter, Transform* aggregateTransform)
		{
			Container = container;
			Mesh = mesh;
			Renderer = renderer;
			RenderMaterial = renderMaterial;
			MeshFilter = meshFilter;
			AggregateTransform = aggregateTransform;
		}

		SceneObjectRef Container;
		SubMesh3DRef Mesh;
		SubMesh3DRendererRef Renderer;
		MaterialRef RenderMaterial;
		Mesh3DFilterRef MeshFilter;
		Transform* AggregateTransform;
	};

	class RenderQueue 
	{
		protected:

		RenderQueueEntry* renderObjects;
		UInt32 id;
		UInt32 realCount;
		UInt32 totalCount;
		UInt32 increaseCount;

		void IncreaseCount(UInt32 count);

		public:

		RenderQueue(UInt32 id, UInt32 initialCount, UInt32 increaseCount);
		~RenderQueue();

		UInt32 GetID();
		void Clear();
		void Add(SceneObjectRef container, SubMesh3DRef mesh, SubMesh3DRendererRef renderer, MaterialRef renderMaterial, Mesh3DFilterRef meshFilter, Transform* aggregateTransform);
		RenderQueueEntry* GetObject(UInt32 index);
		UInt32 GetObjectCount();
	};
}
#endif
