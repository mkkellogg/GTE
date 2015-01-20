/*
 * class: SubMesh3DRendererGL
 *
 * author: Mark Kellogg
 *
 * SubMesh3DRendererGL is an implementation of SubMesh3DRenderer specifically
 * for the OpenGL platform. It implements Render() and RenderShadowVolume(),
 * which are pure virtual functions in SubMesh3DRenderer, and uses OpenGL
 * calls such as glDrawArrays() to carry out the rendering.
 *
 */


#ifndef _GTE_SUBMESH3D_RENDERER_GL_H_
#define _GTE_SUBMESH3D_RENDERER_GL_H_

// forward declarations
class GraphicsGL;
class Graphics;
class VertexAttrBufferGL;
class VertexAttrBuffer;
class Color4Array;
class UV2Array;
class SubMesh3D;
class Material;

#include "object/enginetypes.h"
#include "submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "attributetransformer.h"
#include "geometry/vector/vector3array.h"
#include "geometry/point/point3array.h"

class SubMesh3DRendererGL : public SubMesh3DRenderer
{
	friend class GraphicsGL;

	SubMesh3DRendererGL(AttributeTransformer * attributeTransformer);
	SubMesh3DRendererGL(bool buffersOnGPU, AttributeTransformer * attributeTransformer);
	~SubMesh3DRendererGL();

    public:

    void Render();
    void RenderShadowVolume();
};

#endif

