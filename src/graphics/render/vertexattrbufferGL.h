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

#include "graphics/gl_include.h"
#include "vertexattrbuffer.h"
#include "object/enginetypes.h"

namespace GTE
{
	class VertexAttrBufferGL : public GTE::VertexAttrBuffer
	{
		/*
		 * The constructor and destructor are protected so that no class besides GraphicsGL or SubMesh3DRendererGL can
		 * instantiate or destroy a VertexAttrBufferGL object.
		 */
		friend class SubMesh3DRendererGL;
		friend class GraphicsGL;

		// raw pointer to the buffer data
		Real * data;
		// is this a VBO?
		bool dataOnGPU;
		// OpenGL id for the buffer
		GLuint gpuBufferID;

	protected:

		VertexAttrBufferGL();
		virtual ~VertexAttrBufferGL();

		void Destroy();
		Int32 CalcFullSize() const;
		Int32 CalcFloatCount() const;
		void InitData(const Real * srcData);

	public:

		bool Init(Int32 vertexCount, Int32 componentCount, Int32 stride, bool dataOnGPU, Real *srcData);
		void SetData(const Real * srcData);
		const Real * GetDataPtr() const;
		bool IsGPUBuffer() const;
		GLuint GetGPUBufferID();
		UInt32 GetFullSize() const;
	};
}

#endif
