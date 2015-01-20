/*
 * class: VertexAttrBufferGL
 *
 * author: Mark Kellogg
 *
 * OpenGL-specific implementation of VertexAttrBuffer.
 *
 */


#ifndef _GTE_VERTEX_ATTR_BUFFER_GL_H_
#define _GTE_VERTEX_ATTR_BUFFER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "vertexattrbuffer.h"

class VertexAttrBufferGL : public VertexAttrBuffer
{
    /*
     * The constructor and destructor are protected so that no class besides GraphicsGL or SubMesh3DRendererGL can
     * instantiate or destroy a VertexAttrBufferGL object.
     */
    friend class SubMesh3DRendererGL;
    friend class GraphicsGL;

    // raw pointer to the buffer data
    float * data;
    // is this a VBO?
    bool dataOnGPU;
    // OpenGL id for the buffer
    GLuint gpuBufferID;

    protected:

    VertexAttrBufferGL();
    virtual ~VertexAttrBufferGL();

    void Destroy();
    int CalcFullSize() const;
    int CalcFloatCount() const;

    public:

    bool Init(int vertexCount, int componentCount, int stride, bool dataOnGPU, float *srcData);
    void SetData(const float * data);
    const float * GetDataPtr() const;
    bool IsGPUBuffer() const;
    GLuint GetGPUBufferID();
    unsigned int GetFullSize() const;
};

#endif
