
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "vertexattrbufferGL.h"
#include "global/global.h"
#include "debug/gtedebug.h"

/*
 * Single constructor.
 */
VertexAttrBufferGL::VertexAttrBufferGL() : VertexAttrBuffer(),  data(NULL), dataOnGPU(false), gpuBufferID(0)
{

}

/*
 * Clean-up.
 */
VertexAttrBufferGL::~VertexAttrBufferGL()
{
	Destroy();
}

/*
 * Calculate the number of floating-point entries in the buffer.
 */
int VertexAttrBufferGL::CalcFloatCount() const
{
	return (componentCount + stride) * vertexCount;
}

/*
 * Calculate the total size (in bytes) of the buffer.
 */
int VertexAttrBufferGL::CalcFullSize() const
{
	return CalcFloatCount() * sizeof(float);
}

/*
 * Initialize the buffer.
 *
 * [vertexCount] - The total number of vertices in this buffer.
 * [componentCount] - The number of components in the particular attribute being stored in this buffer (e.g.
 * 					  vertex position = 4 components -> x,y,z,w).
 * [stride] - Padding between vertices, can be used for optimal memory alignment.
 * [dataOnGPU] - Make this a VBO.
 * [srcData] - Data to be copied into the buffer after initialization.
 */
bool VertexAttrBufferGL::Init(int vertexCount, int componentCount, int stride, bool dataOnGPU, float *srcData)
{
	// if this buffer has already be initialized we need to destroy it and start fresh
	Destroy();

	this->componentCount = componentCount;
	this->vertexCount = vertexCount;
	this->stride = stride;

	// calculate number of bytes in the buffer
	int fullDataSize =  CalcFullSize();

	data = new float[CalcFloatCount()];
	ASSERT(data != NULL, "VertexAttrBufferGL::Init -> Could not allocate VertexAttrBufferGL data.", false);

	// zero out the buffer
	if(srcData == NULL)
	{
		memset(data, 0, fullDataSize);
	}

	// create the VBO if necessary
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

/*
 * Copy the data stored in [srcData] into the buffer.
 */
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

/*
 * Deallocate & destroy the buffer.
 */
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

/*
 * Get a pointer to the raw buffer data. This is only a CPU-side pointer;
 * if the buffer is a VBO, this will NOT be a pointer to the GPU memory.
 */
const float * VertexAttrBufferGL::GetDataPtr() const
{
	return (const float *)data;
}

/*
 * Is the buffer a VBO?
 */
bool VertexAttrBufferGL::IsGPUBuffer() const
{
	return dataOnGPU;
}

/*
 *  Get the OpenGL id of the VBO.
 */
GLuint VertexAttrBufferGL::GetGPUBufferID()
{
	return gpuBufferID;
}

/*
 * Public accessor to the full size (in bytes) of the buffer.
 */
unsigned int VertexAttrBufferGL::GetFullSize() const
{
	return CalcFullSize();
}
