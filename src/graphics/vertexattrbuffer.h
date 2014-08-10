#ifndef _VERTEX_ATTR_BUFFER_H_
#define _VERTEX_ATTR_BUFFER_H_

class VertexAttrBuffer
{
    public:

    VertexAttrBuffer();
    virtual ~VertexAttrBuffer();

    virtual void SetData(float * data) = 0;
};

#endif
