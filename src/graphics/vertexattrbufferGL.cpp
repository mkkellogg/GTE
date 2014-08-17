
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "vertexattrbufferGL.h"
#include "ui/debug.h"

int VertexAttrBufferGL::CalcFullSize()
{
	return componentCount * attributeCount * sizeof(float);
}

VertexAttrBufferGL::VertexAttrBufferGL() : VertexAttrBuffer(),  data(NULL), dataOnGPU(false), gpuBufferID(0)
{

}

VertexAttrBufferGL::~VertexAttrBufferGL()
{
	Destroy();
}

bool VertexAttrBufferGL::Init(int attributeCount, int componentCount, bool dataOnGPU, float *srcData)
{
	Destroy();

	this->componentCount = componentCount;
	this->attributeCount = attributeCount;

	int fullDataSize =  CalcFullSize();

	data = new float[componentCount * attributeCount];
	if(data == NULL)
	{
		Debug::PrintMessage("Could not allocate VertexAttrBufferGL data.");
		return false;
	}

	if(srcData == NULL)
	{
		memset(data, 0, fullDataSize);
	}

	if(dataOnGPU)
	{
		glGenBuffers(1, &gpuBufferID);

		if(gpuBufferID > 0)
		{
			if(srcData != NULL)SetData(srcData);
		}
		else this->dataOnGPU = true;
	}

	return true;
}

void VertexAttrBufferGL::SetData(const float * srcData)
{
	int fullDataSize = CalcFullSize();
	memcpy(data, srcData, fullDataSize);

	if(dataOnGPU)
	{
		glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
		glBufferData(GL_ARRAY_BUFFER, fullDataSize, srcData, GL_DYNAMIC_DRAW);
	}
}

void VertexAttrBufferGL::Destroy()
{
	if(dataOnGPU && gpuBufferID)
	{
		glDeleteBuffers(1, &gpuBufferID);
		dataOnGPU = false;
		gpuBufferID = 0;
	}

	if(data != NULL)
	{
		delete data;
		data = NULL;
	}
}

