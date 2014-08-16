
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexattrbuffer.h"

VertexAttrBuffer::VertexAttrBuffer() : componentCount(0), attributeCount(0)
{

}

VertexAttrBuffer::~VertexAttrBuffer()
{


}

int VertexAttrBuffer::GetAttributeCount()
{
	return attributeCount;
}

int VertexAttrBuffer::GetComponentCount()
{
	return componentCount;
}
