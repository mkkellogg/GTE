#ifndef _SUBMESH3D_RENDERER_GL_H_
#define _SUBMESH3D_RENDERER_GL_H_

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

	SubMesh3DRendererGL(Graphics * graphics, AttributeTransformer * attributeTransformer);
	SubMesh3DRendererGL(bool buffersOnGPU, Graphics * graphics, AttributeTransformer * attributeTransformer);
	~SubMesh3DRendererGL();

    public:

    void Render();
};

#endif

