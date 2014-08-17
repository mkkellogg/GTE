#ifndef _VERTEX_ATTR_BUFFER_GL_H_
#define _VERTEX_ATTR_BUFFER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "vertexattrbuffer.h"

class VertexAttrBufferGL : public VertexAttrBuffer
{
    friend class Mesh3DRendererGL;
    friend class GraphicsGL;

    protected:

    float * data;
    bool dataOnGPU;
    GLuint gpuBufferID;

    /*
     * The constructor and destructor are protected so that no class besides GraphicsGL or MeshRendererGL can
     * instantiate or destroy a VertexAttrBufferGL object.
     */
    VertexAttrBufferGL();
    virtual ~VertexAttrBufferGL();

    void Destroy();
    int CalcFullSize();

    public:

    bool Init(int attributeCount, int componentCount, bool dataOnGPU, float *srcData);
    void SetData(const float * data);
};

#endif
