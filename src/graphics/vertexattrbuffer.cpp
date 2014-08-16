
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexattrbuffer.h"

VertexAttrBuffer::VertexAttrBuffer(int componentCount, int attributeCount)
{
	this->componentCount = componentCount;
	this->attributeCount = attributeCount;
}

VertexAttrBuffer::~VertexAttrBuffer()
{


}
