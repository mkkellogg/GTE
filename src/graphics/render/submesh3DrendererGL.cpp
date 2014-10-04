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

	NULL_CHECK_RTRN(containerRenderer,"SubMesh3DRendererGL::Render -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	SHARED_REF_CHECK_RTRN(mesh,"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(doAttributeTransform)
	{
		StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
		   StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
		{
			attributeTransformer->TransformPositions(positionsCopy, transformedPositions);
			SetPositionData(&transformedPositions);
		}

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal) &&
		   StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
		{
			attributeTransformer->TransformNormals(normalsCopy, transformedNormals);
			SetNormalData(&transformedNormals);
		}
	}

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			currentMaterial->SendStandardAttributeBufferToShader(attr, attributeBuffers[i]);
		}
	}

	if(!currentMaterial->VerifySetVars(mesh->GetVertexCount()))return;

	glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
}

