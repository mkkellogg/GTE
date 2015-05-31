
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexattrbuffer.h"

namespace GTE
{
	/*
	 * Single constructor.
	 */
	VertexAttrBuffer::VertexAttrBuffer() : componentCount(0), vertexCount(0), stride(0)
	{

	}

	/*
	 * Clean-up.
	 */
	VertexAttrBuffer::~VertexAttrBuffer()
	{


	}

	/*
	 * Get the number of vertices in this buffer.
	 */
	Int32 VertexAttrBuffer::GetVertexCount() const
	{
		return vertexCount;
	}

	/*
	 * Get the number of components in this buffer's target attribute (e.g. vertex
	 * position has 4 components: x,y,z,w).
	 */
	Int32 VertexAttrBuffer::GetComponentCount() const
	{
		return componentCount;
	}

	/*
	 * Get the padding between vertices.
	 */
	Int32 VertexAttrBuffer::GetStride() const
	{
		return stride;
	}
}
