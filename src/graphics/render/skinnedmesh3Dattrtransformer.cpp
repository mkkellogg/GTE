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
	boneTransformed = NULL;
}

SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes) : AttributeTransformer(attributes)
{
	offset = 0;
	skeleton = NULL;
	vertexBoneMapIndex = -1;
	boneTransformed = NULL;
}

SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
{
	DestroyTransformedBoneCache();
}

void SkinnedMesh3DAttributeTransformer::DestroyTransformedBoneCache()
{
	SAFE_DELETE(boneTransformed);
}

bool SkinnedMesh3DAttributeTransformer::CreateTransformedBoneCache()
{
	if(skeleton != NULL)
	{
		boneTransformed = new unsigned char[skeleton->GetBoneCount()];
		NULL_CHECK(boneTransformed, "SkinnedMesh3DAttributeTransformer::CreateTransformedBoneCache -> Unable to allocate bone transform cache.", false);
		return true;
	}

	return false;
}

void SkinnedMesh3DAttributeTransformer::ClearTransformedBoneCache()
{
	memset(boneTransformed, 0, sizeof(unsigned char) * skeleton->GetBoneCount());
}

void SkinnedMesh3DAttributeTransformer::SetSkeleton(Skeleton * skeleton)
{
	this->skeleton = skeleton;
	DestroyTransformedBoneCache();
	CreateTransformedBoneCache();
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
		ClearTransformedBoneCache();

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

				if(boneTransformed[desc->BoneIndex[b]] == 0)
				{
					bone->TempFullMatrix.SetTo(&bone->OffsetMatrix);

					if(bone->Node->HasTarget())
					{
						const Transform * targetFull = bone->Node->GetFullTransform();
						bone->TempFullMatrix.PreMultiply(targetFull->GetMatrix());
					}

					boneTransformed[desc->BoneIndex[b]] = 1;
				}

				temp.SetTo(&bone->TempFullMatrix);
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
