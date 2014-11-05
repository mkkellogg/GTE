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
#include "geometry/point/point3.h"
#include "geometry/vector/vector3array.h"
#include "geometry/vector/vector3.h"
#include "object/sceneobject.h"
#include "util/time.h"

SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer() : AttributeTransformer()
{
	offset = 0;
	vertexBoneMapIndex = -1;

	boneTransformed = NULL;

	positionTransformedCount = -1;
	positionTransformed = NULL;

	normalTransformedCount = -1;
	normalTransformed = NULL;

	savedTransforms = NULL;
}

SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes) : AttributeTransformer(attributes)
{
	offset = 0;
	vertexBoneMapIndex = -1;
	boneTransformed = NULL;

	positionTransformedCount = -1;
	positionTransformed = NULL;

	normalTransformedCount = -1;
	normalTransformed = NULL;

	savedTransforms = NULL;
}

SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
{
	DestroyTransformedBoneFlagsArray();
	DestroyTransformedPositionFlagsArray();
}

void SkinnedMesh3DAttributeTransformer::DestroySavedTransformsArray()
{
	if(savedTransforms != NULL)
	{
		delete[] savedTransforms;
		savedTransforms = NULL;
	}
}

bool SkinnedMesh3DAttributeTransformer::CreateSavedTransformsArray(unsigned int saveCount)
{

	savedTransforms = new Matrix4x4[saveCount];
	NULL_CHECK(savedTransforms,"SkinnedMesh3DAttributeTransformer::CreateSavedTransformsArray -> Unable to allocate saved transforms array.",false);
	return true;
}

void SkinnedMesh3DAttributeTransformer::DestroyTransformedBoneFlagsArray()
{
	SAFE_DELETE(boneTransformed);
}

bool SkinnedMesh3DAttributeTransformer::CreateTransformedBoneFlagsArray()
{
	if(skeleton.IsValid())
	{
		boneTransformed = new unsigned char[skeleton->GetBoneCount()];
		NULL_CHECK(boneTransformed, "SkinnedMesh3DAttributeTransformer::CreateTransformedBoneCache -> Unable to allocate flags array.", false);
		return true;
	}

	return false;
}

void SkinnedMesh3DAttributeTransformer::ClearTransformedBoneFlagsArray()
{
	memset(boneTransformed, 0, sizeof(unsigned char) * skeleton->GetBoneCount());
}

void SkinnedMesh3DAttributeTransformer::DestroyTransformedPositionFlagsArray()
{
	SAFE_DELETE(positionTransformed);
}

bool SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray(unsigned int positionTransformedCount)
{
	this->positionTransformedCount = positionTransformedCount;
	positionTransformed = new unsigned char[positionTransformedCount];
	NULL_CHECK(positionTransformed, "SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray -> Unable to allocate flags array.", false);

	bool initSuccess = transformedPositions.Init(positionTransformedCount);
	if(!initSuccess)
	{
		Debug::PrintError("SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray -> Could not init transformed vertex array.");
		return false;
	}

	return true;
}

void SkinnedMesh3DAttributeTransformer::ClearTransformedPositionFlagsArray()
{
	memset(positionTransformed, 0, sizeof(unsigned char) * positionTransformedCount);
}

void SkinnedMesh3DAttributeTransformer::DestroyTransformedNormalFlagsArray()
{
	SAFE_DELETE(normalTransformed);
}

bool SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray(unsigned int normalTransformedCount)
{
	this->normalTransformedCount = normalTransformedCount;
	normalTransformed = new unsigned char[normalTransformedCount];
	NULL_CHECK(normalTransformed, "SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray -> Unable to allocate flags array.", false);

	bool initSuccess = transformedNormals.Init(normalTransformedCount);
	if(!initSuccess)
	{
		Debug::PrintError("SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray -> Could not init transformed vertex array.");
		return false;
	}

	return true;
}

void SkinnedMesh3DAttributeTransformer::ClearTransformedNormalFlagsArray()
{
	memset(normalTransformed, 0, sizeof(unsigned char) * normalTransformedCount);
}



void SkinnedMesh3DAttributeTransformer::SetSkeleton(SkeletonRef skeleton)
{
	this->skeleton = skeleton;
	DestroyTransformedBoneFlagsArray();
	CreateTransformedBoneFlagsArray();
}

void SkinnedMesh3DAttributeTransformer::SetVertexBoneMapIndex(int index)
{
	vertexBoneMapIndex = index;
}

void SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut,
																	 const Vector3Array& normalsIn, Vector3Array& normalsOut,
																	 const Point3& centerIn, Point3& centerOut)
{
	positionsIn.CopyTo(&positionsOut);
	normalsIn.CopyTo(&normalsOut);
	centerOut.Set(centerIn.x,centerIn.y,centerIn.z);

	unsigned int uniqueBonesEncountered = 0;
	Matrix4x4 averageBoneOffset;

	if(skeleton.IsValid() && vertexBoneMapIndex >= 0)
	{
		ClearTransformedBoneFlagsArray();

		Matrix4x4 temp;
		Matrix4x4 full;

		VertexBoneMap * vertexBoneMap = skeleton->GetVertexBoneMap(vertexBoneMapIndex);
		NULL_CHECK_RTRN(vertexBoneMap,"SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> No valid vertex bone map found for sub mesh.");

		unsigned int uniqueVertexCount = vertexBoneMap->GetUVertexCount();
		if(positionTransformedCount < 0 || uniqueVertexCount != (unsigned int)positionTransformedCount)
		{
			DestroyTransformedPositionFlagsArray();
			bool createSuccess = CreateTransformedPositionFlagsArray(uniqueVertexCount);

			if(!createSuccess)
			{
				Debug::PrintError("SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create position transform flags array.");
				return;
			}

			DestroyTransformedNormalFlagsArray();
			createSuccess = CreateTransformedNormalFlagsArray(uniqueVertexCount);

			if(!createSuccess)
			{
				Debug::PrintError("SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create normal transform flags array.");
				return;
			}
		}

		ClearTransformedPositionFlagsArray();

		for(unsigned int i = 0; i < positionsOut.GetCount(); i++)
		{
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);

			if(positionTransformed[desc->UVertexIndex] == 1)
			{
				Point3 * p = transformedPositions.GetPoint(desc->UVertexIndex);
				positionsOut.GetPoint(i)->Set(p->x,p->y,p->z);

				Vector3 * v = transformedNormals.GetVector(desc->UVertexIndex);
				normalsOut.GetVector(i)->Set(v->x,v->y,v->z);
			}
			else
			{
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

						averageBoneOffset.Add(&bone->OffsetMatrix);

						boneTransformed[desc->BoneIndex[b]] = 1;
						uniqueBonesEncountered++;
					}

					temp.SetTo(&bone->TempFullMatrix);
					temp.MultiplyByScalar(desc->Weight[b]);
					if(b==0)full.SetTo(&temp);
					else full.Add(&temp);
				}

				Point3 * p = positionsOut.GetPoint(i);
				full.Transform(p);

				Vector3 * v = normalsOut.GetVector(i);
				full.Transform(v);

				transformedPositions.GetPoint(desc->UVertexIndex)->SetTo(p);
				transformedNormals.GetVector(desc->UVertexIndex)->Set(v->x,v->y,v->z);
				positionTransformed[desc->UVertexIndex] = 1;
			}
		}

		if(uniqueBonesEncountered==0)uniqueBonesEncountered=1;
		averageBoneOffset.MultiplyByScalar(1/(float)uniqueBonesEncountered);
		averageBoneOffset.Transform(&centerOut);
	}
}

void SkinnedMesh3DAttributeTransformer::TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut, const Point3& centerIn, Point3& centerOut)
{
	positionsIn.CopyTo(&positionsOut);
	centerOut.Set(centerIn.x,centerIn.y,centerIn.z);

	unsigned int uniqueBonesEncountered = 0;
	Matrix4x4 averageBoneOffset;

	if(skeleton.IsValid() && vertexBoneMapIndex >= 0)
	{
		ClearTransformedBoneFlagsArray();

		Matrix4x4 temp;
		Matrix4x4 full;

		VertexBoneMap * vertexBoneMap = skeleton->GetVertexBoneMap(vertexBoneMapIndex);
		NULL_CHECK_RTRN(vertexBoneMap,"SkinnedMesh3DAttributeTransformer::TransformPositions -> No valid vertex bone map found for sub mesh.");

		unsigned int uniqueVertexCount = vertexBoneMap->GetUVertexCount();
		if(positionTransformedCount < 0 || uniqueVertexCount != (unsigned int)positionTransformedCount)
		{
			DestroyTransformedPositionFlagsArray();
			bool createSuccess = CreateTransformedPositionFlagsArray(uniqueVertexCount);

			if(!createSuccess)
			{
				Debug::PrintError("SkinnedMesh3DAttributeTransformer::TransformPositions -> Unable to create position transform flags array.");
				return;
			}
		}

		ClearTransformedPositionFlagsArray();

		for(unsigned int i = 0; i < positionsOut.GetCount(); i++)
		{
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);
			if(positionTransformed[desc->UVertexIndex] == 1)
			{
				Point3 * p = transformedPositions.GetPoint(desc->UVertexIndex);
				positionsOut.GetPoint(i)->Set(p->x,p->y,p->z);
			}
			else
			{
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

						averageBoneOffset.Add(&bone->OffsetMatrix);
						boneTransformed[desc->BoneIndex[b]] = 1;
					}

					temp.SetTo(&bone->TempFullMatrix);
					temp.MultiplyByScalar(desc->Weight[b]);
					if(b==0)full.SetTo(&temp);
					else full.Add(&temp);
				}

				Point3 * p = positionsOut.GetPoint(i);
				full.Transform(p);

				transformedPositions.GetPoint(desc->UVertexIndex)->Set(p->x,p->y,p->z);
				positionTransformed[desc->UVertexIndex] = 1;
			}
		}

		if(uniqueBonesEncountered==0)uniqueBonesEncountered=1;
		averageBoneOffset.MultiplyByScalar(1/(float)uniqueBonesEncountered);
		averageBoneOffset.Transform(&centerOut);
	}
}

void SkinnedMesh3DAttributeTransformer::TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut)
{
	normalsIn.CopyTo(&normalsOut);
}
