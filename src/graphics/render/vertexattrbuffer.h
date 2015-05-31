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

#include "object/enginetypes.h"

namespace GTE
{
	class VertexAttrBuffer
	{
	protected:

		// number of components per attribute, e.g. vertex positions have
		// a component count of 4 (x,y,z,w)
		Int32 componentCount;
		// total number of vertices in the buffer
		Int32 vertexCount;
		// padding space between attributes, can be used to achieve optimal memory alignment
		Int32 stride;

	public:

		VertexAttrBuffer();
		virtual ~VertexAttrBuffer();

		virtual Bool Init(Int32 vertexCount, Int32 componentCount, Int32 stride, Bool dataOnGPU, Real *srcData) = 0;
		virtual void SetData(const Real * data) = 0;
		Int32 GetVertexCount() const;
		Int32 GetComponentCount() const;
		Int32 GetStride() const;
		virtual UInt32 GetFullSize() const = 0;
	};
}

#endif
