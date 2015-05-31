
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "graphics/gl_include.h"
#include "vertexattrbufferGL.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	 * Single constructor.
	 */
	VertexAttrBufferGL::VertexAttrBufferGL() : VertexAttrBuffer(), data(NULL), dataOnGPU(false), gpuBufferID(0)
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
	 * Calculate the number of Realing-point entries in the buffer.
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
		return CalcFloatCount() * sizeof(Real);
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
	bool VertexAttrBufferGL::Init(int vertexCount, int componentCount, int stride, bool dataOnGPU, Real *srcData)
	{
		// if this buffer has already be initialized we need to destroy it and start fresh
		Destroy();

		this->componentCount = componentCount;
		this->vertexCount = vertexCount;
		this->stride = stride;

		// calculate number of bytes in the buffer
		int fullDataSize = CalcFullSize();

		data = new Real[CalcFloatCount()];
		ASSERT(data != NULL, "VertexAttrBufferGL::Init -> Could not allocate VertexAttrBufferGL data.");

		// zero out the buffer
		if (srcData == NULL)
		{
			memset(data, 0, fullDataSize);
		}

		// create the VBO if necessary
		if (dataOnGPU)
		{
			glGenBuffers(1, &gpuBufferID);

			if (gpuBufferID > 0)
			{
				this->dataOnGPU = true;
			}
			else this->dataOnGPU = false;
		}

		if (srcData != NULL)InitData(srcData);

		return true;
	}

	void VertexAttrBufferGL::InitData(const Real * srcData)
	{
		int fullDataSize = CalcFullSize();
		memcpy(data, srcData, fullDataSize);

		if (dataOnGPU)
		{
			glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
			glBufferData(GL_ARRAY_BUFFER, fullDataSize, NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, fullDataSize, srcData);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	/*
	 * Copy the data stored in [srcData] into the buffer.
	 */
	void VertexAttrBufferGL::SetData(const Real * srcData)
	{
		int fullDataSize = CalcFullSize();

		if (dataOnGPU)
		{
			glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
			glBufferData(GL_ARRAY_BUFFER, fullDataSize, NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, fullDataSize, srcData);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// below is the memory-mapping approach to updating
			// VBOs. empirically this has shown to be slower.

			/*	glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
				glBufferData(GL_ARRAY_BUFFER, fullDataSize, NULL, GL_DYNAMIC_DRAW);
				Real* ptr = (Real*)glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				if (ptr)
				{
				memcpy(ptr, srcData, fullDataSize);
				glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
				}*/
		}
		else memcpy(data, srcData, fullDataSize);
	}

	/*
	 * Deallocate & destroy the buffer.
	 */
	void VertexAttrBufferGL::Destroy()
	{
		if (dataOnGPU && gpuBufferID)
		{
			glDeleteBuffers(1, &gpuBufferID);
			dataOnGPU = false;
			gpuBufferID = 0;
		}

		if (data != NULL)
		{
			delete data;
			data = NULL;
		}
	}

	/*
	 * Get a pointer to the raw buffer data. This is only a CPU-side pointer;
	 * if the buffer is a VBO, this will NOT be a pointer to the GPU memory.
	 */
	const Real * VertexAttrBufferGL::GetDataPtr() const
	{
		return (const Real *)data;
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
	UInt32 VertexAttrBufferGL::GetFullSize() const
	{
		return CalcFullSize();
	}
}