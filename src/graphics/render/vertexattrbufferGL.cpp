#include "graphics/gl_include.h"
#include "vertexattrbufferGL.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

#include <memory.h>

namespace GTE {
    /*
     * Single constructor.
     */
    VertexAttrBufferGL::VertexAttrBufferGL() : VertexAttrBuffer(), data(nullptr), dataOnGPU(false), gpuBufferID(0) {

    }

    /*
     * Clean-up.
     */
    VertexAttrBufferGL::~VertexAttrBufferGL() {
        Destroy();
    }

    /*
     * Calculate the number of floating-point entries in the buffer.
     */
    UInt32 VertexAttrBufferGL::CalcTotalFloatCount() const {
        return (componentCount + stride) * totalVertexCount;
    }

    /*
     * Calculate the total size (in bytes) of the buffer.
     */
    UInt32 VertexAttrBufferGL::CalcTotalFullSize() const {
        return CalcTotalFloatCount() * sizeof(Real);
    }

    /*
    * Calculate the number of floating-point entries to be rendered.
    */
    UInt32 VertexAttrBufferGL::CalcRenderFloatCount() const {
        return (componentCount + stride) * renderVertexCount;
    }

    /*
    * Calculate the size (in bytes) of the buffer to be rendered.
    */
    UInt32 VertexAttrBufferGL::CalcRenderFullSize() const {
        return CalcRenderFloatCount() * sizeof(Real);
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
    Bool VertexAttrBufferGL::Init(UInt32 totalVertexCount, UInt32 componentCount, UInt32 stride, Bool dataOnGPU, const Real *srcData) {
        // if this buffer has already be initialized we need to destroy it and start fresh
        Destroy();

        this->componentCount = componentCount;
        this->totalVertexCount = totalVertexCount;
        this->renderVertexCount = totalVertexCount;
        this->stride = stride;

        // calculate number of bytes in the buffer
        Int32 fullDataSize = CalcTotalFullSize();

        data = new(std::nothrow) Real[CalcTotalFloatCount()];
        ASSERT(data != nullptr, "VertexAttrBufferGL::Init -> Could not allocate VertexAttrBufferGL data.");

        // zero out the buffer
        if (srcData == nullptr) {
            memset(data, 0, fullDataSize);
        }

        // create the VBO if necessary
        if (dataOnGPU) {
            glGenBuffers(1, &gpuBufferID);

            if (gpuBufferID > 0) {
                this->dataOnGPU = true;
            }
            else this->dataOnGPU = false;
        }

        if (srcData != nullptr)InitData(srcData);

        return true;
    }

    void VertexAttrBufferGL::InitData(const Real * srcData) {
        Int32 fullDataSize = CalcTotalFullSize();

        if (dataOnGPU) {
            glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
            glBufferData(GL_ARRAY_BUFFER, fullDataSize, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, fullDataSize, srcData);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else memcpy(data, srcData, fullDataSize);
    }

    /*
     * Copy the data stored in [srcData] into the buffer.
     */
    void VertexAttrBufferGL::SetData(const Real * srcData) {
        Int32 fullDataSize = CalcRenderFullSize();

        if (dataOnGPU) {
            glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
            glBufferData(GL_ARRAY_BUFFER, fullDataSize, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, fullDataSize, srcData);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // below is the memory-mapping approach to updating
            // VBOs. empirically this has shown to be slower.

            /*	glBindBuffer(GL_ARRAY_BUFFER, gpuBufferID);
                glBufferData(GL_ARRAY_BUFFER, fullDataSize, nullptr, GL_DYNAMIC_DRAW);
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
    void VertexAttrBufferGL::Destroy() {
        if (dataOnGPU && gpuBufferID) {
            glDeleteBuffers(1, &gpuBufferID);
            dataOnGPU = false;
            gpuBufferID = 0;
        }

        SAFE_DELETE_ARRAY(data);
    }

    /*
     * Get a pointer to the raw buffer data. This is only a CPU-side pointer;
     * if the buffer is a VBO, this will NOT be a pointer to the GPU memory.
     */
    Real * VertexAttrBufferGL::GetDataPtr() {
        return data;
    }

    /*
    * Get a const pointer to the raw buffer data. This is only a CPU-side pointer;
    * if the buffer is a VBO, this will NOT be a pointer to the GPU memory.
    */
    const Real * VertexAttrBufferGL::GetConstDataPtr() const {
        return (const Real *)data;
    }

    /*
     * Is the buffer a VBO?
     */
    Bool VertexAttrBufferGL::IsGPUBuffer() const {
        return dataOnGPU;
    }

    /*
     *  Get the OpenGL id of the VBO.
     */
    GLuint VertexAttrBufferGL::GetGPUBufferID() const {
        return gpuBufferID;
    }
}
