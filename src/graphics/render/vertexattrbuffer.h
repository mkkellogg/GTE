#ifndef _GTE_VERTEX_ATTR_BUFFER_H_
#define _GTE_VERTEX_ATTR_BUFFER_H_

class VertexAttrBuffer
{
	protected:

	int componentCount;
	int vertexCount;
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
