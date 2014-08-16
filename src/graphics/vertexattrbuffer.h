#ifndef _VERTEX_ATTR_BUFFER_H_
#define _VERTEX_ATTR_BUFFER_H_

class VertexAttrBuffer
{
	protected:

	int componentCount;
	int attributeCount;

    public:

    VertexAttrBuffer();
    virtual ~VertexAttrBuffer();

    virtual bool Init(int attributeCount, int componentCount, bool dataOnGPU, float *srcData) = 0;
    virtual void SetData(const float * data) = 0;
    int GetAttributeCount();
    int GetComponentCount();
};

#endif
