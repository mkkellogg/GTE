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
	float realTime = Time::GetRealTimeSinceStartup();
	unsigned int intTime = (unsigned int)Time::GetRealTimeSinceStartup();
	unsigned int mod = intTime % 2;
	float fraction = realTime - (float)intTime;
	float mag = .001;

	float scaleFactor = 1;

	if(mod == 0)
	{
		offset += Time::GetDeltaTime();
		scaleFactor = 1 + offset;
	}
	else
	{
		offset -= Time::GetDeltaTime();
		scaleFactor = (1+mag) - offset;
	}

	Transform transform;
	//transform.Scale(scaleFactor,scaleFactor,scaleFactor, true);

	positionsIn.CopyTo(&positionsOut);

	if(skeleton != NULL && vertexBoneMapIndex >= 0)
	{
		Matrix4x4 temp;
		Matrix4x4 full;
		for(unsigned int i = 0; i < positionsOut.GetCount(); i++)
		{
			float weightSum = 0;
			full.SetIdentity();
			VertexBoneMap::VertexMappingDescriptor *desc = skeleton->GetVertexBoneMap(vertexBoneMapIndex)->GetDescriptor(i);
			for(unsigned int b = 0; b < desc->BoneCount; b++)
			{
				Bone * bone = skeleton->GetBone(desc->BoneIndex[b]);
				temp.SetTo(&(bone->OffsetMatrix));
				if(((SceneObjectSkeletonNode*)(bone->Node))->GetTarget().IsValid())
				{
					//SceneObjectTransform preFull;
					//((SceneObjectSkeletonNode*)(bone->Node))->GetTarget()->GetFullTransform(&preFull);

					const Transform * targetFull = bone->Node->GetFullTransform();
					temp.PreMultiply(targetFull->GetMatrix());

					//temp.PreMultiply(preFull.GetMatrix());
				}
				else
				{
					//printf("no target for: %s\n",bone->Name.c_str());
				}
				temp.MultiplyByScalar(desc->Weight[b]);
				if(b==0)full.SetTo(&temp);
				else full.Add(&temp);
				weightSum+=desc->Weight[b];
			}
			//if(abs(weightSum - 1) > .05 && desc->BoneCount > 0)printf("weights are messed!\n");
			full.Transform(positionsOut.GetPoint(i));
		}
	}
}

void SkinnedMesh3DAttributeTransformer::TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut)
{
	normalsIn.CopyTo(&normalsOut);
}