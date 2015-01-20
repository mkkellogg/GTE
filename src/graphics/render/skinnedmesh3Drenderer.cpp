#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "skinnedmesh3Drenderer.h"
#include "submesh3Drenderer.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/animation/vertexbonemap.h"
#include "graphics/stdattributes.h"
#include "graphics/animation/skeleton.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
#include "global/global.h"
#include "debug/gtedebug.h"

/*
 * Default constructor.
 */
SkinnedMesh3DRenderer::SkinnedMesh3DRenderer()
{

}

/*
 * Clean-up.
 */
SkinnedMesh3DRenderer::~SkinnedMesh3DRenderer()
{
	// loop through each VertexBoneMap and delete
	for(unsigned int i = 0; i < vertexBoneMaps.size(); i++)
	{
		VertexBoneMap * map = vertexBoneMaps[i];
		SAFE_DELETE(map);
		vertexBoneMaps[i] = NULL;
	}
	vertexBoneMaps.clear();
}

/*
 * Set the skeleton for this renderer.
 */
void SkinnedMesh3DRenderer::SetSkeleton(SkeletonRef skeleton)
{
	this->skeleton = skeleton;
}

/*
 * Get a reference to this renderer's skeleton.
 */
SkeletonRef SkinnedMesh3DRenderer::GetSkeleton()
{
	return skeleton;
}

/*
 * @Override Mesh3DRenderer::UpdateFromMesh()
 *
 * This method first calls the overridden parent method. It extends the functionality of that method by
 * initializing an instance of SkinnedMesh3DAttributeTransformer for each sub-mesh in the target mesh,
 * to perform vertex skinning for each.
 */
void SkinnedMesh3DRenderer::UpdateFromMesh()
{
	// get the target mesh
	Mesh3DRef mesh = GetTargetMesh();
	//Mesh3DRef mesh = sceneObject->GetMesh3D();

	if(mesh.IsValid())
	{
		// call the base method
		Mesh3DRenderer::UpdateFromMesh(mesh);

		// loop through each sub-mesh in the target mesh
		for(unsigned int i = 0; i< mesh->GetSubMeshCount(); i++)
		{
			int vertexBoneMapIndex = -1;
			SubMesh3DRendererRef subRenderer = this->GetSubRenderer(i);

			// does the sub-mesh at index [i] have a corresponding VertexBoneMap object?
			if(subMeshIndexMap.find(i) != subMeshIndexMap.end())
			{
				vertexBoneMapIndex = subMeshIndexMap[i];

				// only create  SkinnedMesh3DAttributeTransformer for the sub-mesh at [i]
				// if has not already been done
				if(subRenderer->GetAttributeTransformer() == NULL)
				{
					// setup the attribute transformer to transform both normals and positions
					StandardAttributeSet attributes = StandardAttributes::CreateAttributeSet();
					StandardAttributes::AddAttribute(&attributes, StandardAttribute::Position);
					StandardAttributes::AddAttribute(&attributes, StandardAttribute::Normal);

					SkinnedMesh3DAttributeTransformer *attributeTransformer = new SkinnedMesh3DAttributeTransformer(attributes);
					ASSERT_RTRN(attributeTransformer != NULL,"SkinnedMesh3DRenderer::UpdateFromMesh -> Could not allocate attribute transformer.");

					subRenderer->SetAttributeTransformer(attributeTransformer);
				}
			}
			else // since no vertex bone map exists for this sub-mesh, set the attribute transformer to NULL
			{
				subRenderer->SetAttributeTransformer(NULL);
			}

			// update the attribute transformer
			if(subRenderer->GetAttributeTransformer() != NULL)
			{
				SkinnedMesh3DAttributeTransformer * attrTransformer = dynamic_cast<SkinnedMesh3DAttributeTransformer*>(subRenderer->GetAttributeTransformer());
				if(attrTransformer != NULL)
				{
					attrTransformer->SetRenderer(this);
					attrTransformer->SetVertexBoneMapIndex(vertexBoneMapIndex);
				}
			}
		}
	}
}

/*
 * Add a VertexBoneMap object to this renderer.
 */
void SkinnedMesh3DRenderer::AddVertexBoneMap(VertexBoneMap * map)
{
	vertexBoneMaps.push_back(map);
}

/*
 * Retrieve the VertexBoneMap object stored at [index] in [vertexBoneMaps].
 */
VertexBoneMap * SkinnedMesh3DRenderer::GetVertexBoneMap(unsigned int index)
{
	ASSERT(index < vertexBoneMaps.size(), "Mesh3D::GetVertexBoneMap -> Index out of range.", NULL);

	return vertexBoneMaps[index];
}

/*
 * Map the sub-mesh at [subMeshIndex] in the target mesh of this renderer to the VertexBoneMap structure at
 * [vertexBoneMapIndex] in the member list of VertexBoneMap objects: [vertexBoneMaps]. This establishes the link
 * between
 */
void SkinnedMesh3DRenderer::MapSubMeshToVertexBoneMap(unsigned int subMeshIndex, unsigned int vertexBoneMapIndex)
{
	subMeshIndexMap[subMeshIndex] = vertexBoneMapIndex;
}
