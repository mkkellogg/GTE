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

#include "engine.h"
#include "base/binarymask.h"
#include "object/engineobject.h"

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
			Container = nullptr;
			Mesh = nullptr;
			Renderer = nullptr;
			RenderMaterial = nullptr;
			MeshFilter = nullptr;
		}

		RenderQueueEntry(SceneObject* container, SubMesh3D* mesh, SubMesh3DRenderer* renderer, MaterialSharedPtr* renderMaterial, Mesh3DFilter* meshFilter, Transform* aggregateTransform)
		{
			Container = container;
			Mesh = mesh;
			Renderer = renderer;
			RenderMaterial = renderMaterial;
			MeshFilter = meshFilter;
		}

		SceneObject* Container;
		SubMesh3D* Mesh;
		SubMesh3DRenderer* Renderer;
		MaterialSharedPtr* RenderMaterial;
		Mesh3DFilter* MeshFilter;
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
		void Add(SceneObject* container, SubMesh3D* mesh, SubMesh3DRenderer* renderer, MaterialSharedPtr* renderMaterial, Mesh3DFilter* meshFilter, Transform* aggregateTransform);
		RenderQueueEntry* GetObject(UInt32 index);
		UInt32 GetObjectCount();
	};
}
#endif
