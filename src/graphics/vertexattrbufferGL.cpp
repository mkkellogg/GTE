
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "vertexattrbufferGL.h"
#include "ui/debug.h"

VertexAttrBufferGL::VertexAttrBufferGL(int componentCount, int attributeCount) : VertexAttrBuffer(componentCount, attributeCount), data(NULL), dataOnGPU(false)
{

}

VertexAttrBufferGL::~VertexAttrBufferGL()
{
	Destroy();
}

bool VertexAttrBufferGL::Init(bool dataOnGPU, float *srcData)
{
	int fullDataSize =  componentCount * attributeCount * sizeof(float);

	data = new float[componentCount * attributeCount];
	if(data == NULL)
	{
		Debug::PrintMessage("Could not allocate VertexAttrBufferGL data.");
		return false;
	}

	if(srcData != NULL)
	{
		memcpy(data, srcData, fullDataSize);
	}

	if(dataOnGPU)
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		if(vbo > 0)
		{
			if(srcData != NULL)
			{
				glBufferData(GL_ARRAY_BUFFER, fullDataSize, srcData, GL_DYNAMIC_DRAW);
			}
		}
		else this->dataOnGPU = true;
	}


	return true;
}

void VertexAttrBufferGL::SetData(const float * data)
{

}

void VertexAttrBufferGL::Destroy()
{
	if(data != NULL)
	{
		delete data;
		data = NULL;
	}
}
