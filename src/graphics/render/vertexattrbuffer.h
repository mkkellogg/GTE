/*
 * class: VertexAttrBuffer
 *
 * author: Mark Kellogg
 *
 * Base class for vertex attribute buffers, which are buffers that are meant
 * to contain mesh attribute data (vertex positions, vertex normals, UV coordinates, etc.)
 * and be structured so that they can be directly transferred to the GPU.
 *
 * Since the required organization of such a data structure may conceivably vary from
 * platform to platform (e.g. from OpenGL to DirectX), VertexAttrBuffer is designed to
 * have its platform specific implementation in a deriving class.
 *
 */

#ifndef _GTE_VERTEX_ATTR_BUFFER_H_
#define _GTE_VERTEX_ATTR_BUFFER_H_

#include "engine.h"

namespace GTE
{
	class VertexAttrBuffer
	{
	protected:

		// number of components per attribute, e.g. vertex positions have
		// a component count of 4 (x,y,z,w)
		UInt32 componentCount;
		// total number of vertices in the buffer
		UInt32 totalVertexCount;
		// total number of vertices to be rendered
		UInt32 renderVertexCount;
		// padding space between attributes, can be used to achieve optimal memory alignment
		UInt32 stride;

	public:

		VertexAttrBuffer();
		virtual ~VertexAttrBuffer();

		virtual Bool Init(UInt32 totalVertexCount, UInt32 componentCount, UInt32 stride, Bool dataOnGPU, const Real *srcData) = 0;
		virtual void SetData(const Real * data) = 0;
		Int32 GetTotalVertexCount() const;
		void SetRenderVertexCount(UInt32 count);
		Int32 GetRenderVertexCount() const;
		Int32 GetComponentCount() const;
		Int32 GetStride() const;
	};
}

#endif
