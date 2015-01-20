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
#include "debug/gtedebug.h"

/*
 * Constructor with pointer to an attribute transformer, buffersOnGPU = false by default
 */
SubMesh3DRendererGL::SubMesh3DRendererGL(AttributeTransformer * attributeTransformer) : SubMesh3DRendererGL(false, attributeTransformer)
{

}

/*
 * Constructor with pointer to an attribute transformer, and parameter to choose CPU-side or GPU-side vertex attribute buffers
 */
SubMesh3DRendererGL::SubMesh3DRendererGL(bool buffersOnGPU, AttributeTransformer * attributeTransformer) : SubMesh3DRenderer(buffersOnGPU, attributeTransformer)
{

}

/*
 * Clean-up
 */
SubMesh3DRendererGL::~SubMesh3DRendererGL()
{

}

/*
 * Implement rendering of the target sub-mesh with OpenGL calls.
 */
void SubMesh3DRendererGL::Render()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::Render -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

	MaterialRef currentMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();

	// send the target sub-mesh's attributes to the shader
	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			currentMaterial->SendStandardAttributeBufferToShader(attr, attributeBuffers[i]);
		}
	}

	// make sure all required shader variables have been set correctly
	if(!ValidateMaterial(currentMaterial))return;

	unsigned int totalCount = mesh->GetTotalVertexCount();

	// render the mesh
	glDrawArrays(GL_TRIANGLES, 0, totalCount);
}

/*
 * Implement rendering of the target sub-mesh's shadow volume with OpenGL calls.
 */
void SubMesh3DRendererGL::RenderShadowVolume()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::RenderShadowVolume -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::RenderShadowVolume -> Could not find matching sub mesh for sub renderer.");

	MaterialRef currentMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();

	if(shadowVolumePositions.GetCount() > 0)
	{
		// set the shadow volume vertex attribute buffer data
		SetShadowVolumePositionData(&shadowVolumePositions);
		// send shadow volume vertex positions to the shader
		currentMaterial->SendStandardAttributeBufferToShader(StandardAttribute::ShadowPosition, attributeBuffers[(int)StandardAttribute::ShadowPosition]);
		// render the shadow volume
		glDrawArrays(GL_TRIANGLES, 0, shadowVolumePositions.GetCount());
	}
}

