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

class VertexAttrBuffer
{
	protected:

	// number of components per attribute, e.g. vertex positions have
	// a component count of 4 (x,y,z,w)
	int componentCount;
	// total number of vertices in the buffer
	int vertexCount;
	// padding space between attributes, can be used to achieve optimal memory alignment
	int stride;

    public:

    VertexAttrBuffer();
    virtual ~VertexAttrBuffer();

    virtual bool Init(int vertexCount, int componentCount, int stride, bool dataOnGPU, float *srcData) = 0;
    virtual void SetData(const float * data) = 0;
    int GetVertexCount();
    int GetComponentCount();
    int GetStride();
    virtual unsigned int GetFullSize() const = 0;
};

#endif
