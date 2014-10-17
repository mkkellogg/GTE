#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "graphics/stdattributes.h"
#include "graphics/animation/vertexbonemap.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/sceneobjectskeletonnode.h"
#include "graphics/animation/bone.h"
#include "base/basevector4array.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "object/sceneobject.h"
#include "util/time.h"

SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer() : AttributeTransformer()
{
	offset = 0;
	skeleton = NULL;
	vertexBoneMapIndex = -1;
}

SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes) : AttributeTransformer(attributes)
{
	offset = 0;
	skeleton = NULL;
	vertexBoneMapIndex = -1;
}

SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
{

}

void SkinnedMesh3DAttributeTransformer::SetSkeleton(Skeleton * skeleton)
{
	this->skeleton = skeleton;
}
void SkinnedMesh3DAttributeTransformer::SetVertexBoneMapIndex(int index)
{
	vertexBoneMapIndex = index;
}

void SkinnedMesh3DAttributeTransformer::TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut)
{
	positionsIn.CopyTo(&positionsOut);

	if(skeleton != NULL && vertexBoneMapIndex >= 0)
	{
		Matrix4x4 temp;
		Matrix4x4 full;

		VertexBoneMap * vertexBoneMap = skeleton->GetVertexBoneMap(vertexBoneMapIndex);
		NULL_CHECK_RTRN(vertexBoneMap,"SkinnedMesh3DAttributeTransformer::TransformPositions -> No valid vertex bone map found for sub mesh.");

		for(unsigned int i = 0; i < positionsOut.GetCount(); i++)
		{
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);
			if(desc->BoneCount == 0)full.SetIdentity();
			for(unsigned int b = 0; b < desc->BoneCount; b++)
			{
				Bone * bone = skeleton->GetBone(desc->BoneIndex[b]);
				temp.SetTo(&(bone->OffsetMatrix));

				if(bone->Node->HasTarget())
				{
					const Transform * targetFull = bone->Node->GetFullTransform();
					temp.PreMultiply(targetFull->GetMatrix());
				}

				temp.MultiplyByScalar(desc->Weight[b]);
				if(b==0)full.SetTo(&temp);
				else full.Add(&temp);
			}

			full.Transform(positionsOut.GetPoint(i));
		}
	}
}

void SkinnedMesh3DAttributeTransformer::TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut)
{
	normalsIn.CopyTo(&normalsOut);
}
