
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexattrbuffer.h"

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
int VertexAttrBuffer::GetVertexCount()
{
	return vertexCount;
}

/*
 * Get the number of components in this buffer's target attribute (e.g. vertex
 * position has 4 components: x,y,z,w).
 */
int VertexAttrBuffer::GetComponentCount()
{
	return componentCount;
}

/*
 * Get the padding between vertices.
 */
int VertexAttrBuffer::GetStride()
{
	return stride;
}
