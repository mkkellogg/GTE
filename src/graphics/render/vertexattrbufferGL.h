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

#include "engine.h"
#include "graphics/gl_include.h"
#include "vertexattrbuffer.h"

namespace GTE {
    class VertexAttrBufferGL : public GTE::VertexAttrBuffer {
        friend class SubMesh3DRendererGL;
        // necessary during rendering
        friend class GraphicsGL;

        // raw pointer to the buffer data
        Real * data;
        // is this a VBO?
        Bool dataOnGPU;
        // OpenGL id for the buffer
        GLuint gpuBufferID;

    protected:

        VertexAttrBufferGL();
        virtual ~VertexAttrBufferGL();

        void Destroy();
        UInt32 CalcTotalFullSize() const;
        UInt32 CalcTotalFloatCount() const;
        UInt32 CalcRenderFullSize() const;
        UInt32 CalcRenderFloatCount() const;
        void InitData(const Real * srcData);

    public:

        Bool Init(UInt32 vertexCount, UInt32 componentCount, UInt32 stride, Bool dataOnGPU, const Real *srcData);
        void SetData(const Real * srcData);
        Real * GetDataPtr();
        const Real * GetConstDataPtr() const;
        Bool IsGPUBuffer() const;
        GLuint GetGPUBufferID() const;
    };
}

#endif
