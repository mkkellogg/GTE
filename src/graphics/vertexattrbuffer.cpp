
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexattrbuffer.h"

VertexAttrBuffer::VertexAttrBuffer() : componentCount(0), vertexCount(0), stride(0)
{

}

VertexAttrBuffer::~VertexAttrBuffer()
{


}

int VertexAttrBuffer::GetVertexCount()
{
	return vertexCount;
}

int VertexAttrBuffer::GetComponentCount()
{
	return componentCount;
}

int VertexAttrBuffer::GetStride()
{
	return stride;
}
