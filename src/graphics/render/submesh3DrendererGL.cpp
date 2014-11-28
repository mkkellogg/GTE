#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "submesh3DrendererGL.h"
#include "vertexattrbufferGL.h"
#include "graphics/stdattributes.h"
#include "graphics/graphicsGL.h"
#include "graphics/shader/shaderGL.h"
#include "submesh3Drenderer.h"
#include "mesh3Drenderer.h"
#include "material.h"
#include "graphics/object/submesh3D.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "attributetransformer.h"
#include "global/global.h"
#include "ui/debug.h"


SubMesh3DRendererGL::SubMesh3DRendererGL(Graphics * graphics, AttributeTransformer * attributeTransformer) : SubMesh3DRendererGL(false, graphics, attributeTransformer)
{

}

SubMesh3DRendererGL::SubMesh3DRendererGL(bool buffersOnGPU, Graphics * graphics, AttributeTransformer * attributeTransformer) : SubMesh3DRenderer(buffersOnGPU, graphics, attributeTransformer)
{

}

SubMesh3DRendererGL::~SubMesh3DRendererGL()
{

}

void SubMesh3DRendererGL::Render()
{
	MaterialRef currentMaterial = graphics->GetActiveMaterial();
	UseMaterial(currentMaterial);

	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::Render -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			currentMaterial->SendStandardAttributeBufferToShader(attr, attributeBuffers[i]);
		}
	}

	if(!currentMaterial->VerifySetVars(mesh->GetTotalVertexCount()))return;

	glDrawArrays(GL_TRIANGLES, 0, mesh->GetTotalVertexCount());
}

