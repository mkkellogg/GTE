#ifndef _VERTEX_ATTR_BUFFER_GL_H_
#define _VERTEX_ATTR_BUFFER_GL_H_

#include "vertexattrbuffer.h"

class VertexAttrBufferGL : public VertexAttrBuffer
{
    friend class GraphicsGL;

    protected:

    VertexAttrBufferGL();
    virtual ~VertexAttrBufferGL();

    public:

    void SetData(const float * data);
};

#endif
