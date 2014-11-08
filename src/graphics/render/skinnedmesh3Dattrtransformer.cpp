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

/*
 * Default constructor, initialize all member variables to default values.
 */
SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer() : AttributeTransformer()
{
	vertexBoneMapIndex = -1;

	boneTransformed = NULL;

	positionTransformedCount = -1;
	positionTransformed = NULL;

	normalTransformedCount = -1;
	normalTransformed = NULL;

	savedTransforms = NULL;
}

/*
 * Parameterized constructor - indicated which mesh attributes will be transformed via [attributes].
 */
SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes) : AttributeTransformer(attributes)
{
	vertexBoneMapIndex = -1;
	boneTransformed = NULL;

	positionTransformedCount = -1;
	positionTransformed = NULL;

	normalTransformedCount = -1;
	normalTransformed = NULL;

	savedTransforms = NULL;
}

/*
 * Cleanup.
 */
SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
{
	DestroyTransformedBoneFlagsArray();
	DestroyTransformedPositionFlagsArray();
}

/*
 * Destroy the [savedTransforms] member array.
 */
void SkinnedMesh3DAttributeTransformer::DestroySavedTransformsArray()
{
	if(savedTransforms != NULL)
	{
		delete[] savedTransforms;
		savedTransforms = NULL;
	}
}

/*
 * Allocate the [savedTransforms] member array.
 */
bool SkinnedMesh3DAttributeTransformer::CreateSavedTransformsArray(unsigned int saveCount)
{
	savedTransforms = new Matrix4x4[saveCount];
	ASSERT(savedTransforms != NULL,"SkinnedMesh3DAttributeTransformer::CreateSavedTransformsArray -> Unable to allocate saved transforms array.",false);
	return true;
}

/*
 * Destroy the array of bone transformation flags in [boneTransformed].
 */
void SkinnedMesh3DAttributeTransformer::DestroyTransformedBoneFlagsArray()
{
	SAFE_DELETE(boneTransformed);
}

/*
 * Create the bone transformation flags array [boneTransformed].
 */
bool SkinnedMesh3DAttributeTransformer::CreateTransformedBoneFlagsArray()
{
	if(skeleton.IsValid())
	{
		boneTransformed = new unsigned char[skeleton->GetBoneCount()];
		ASSERT(boneTransformed != NULL, "SkinnedMesh3DAttributeTransformer::CreateTransformedBoneCache -> Unable to allocate flags array.", false);
		return true;
	}

	return false;
}

/*
 * Clear the bone transformation array.
 */
void SkinnedMesh3DAttributeTransformer::ClearTransformedBoneFlagsArray()
{
	memset(boneTransformed, 0, sizeof(unsigned char) * skeleton->GetBoneCount());
}

/*
 * Destroy the position transformation flag array [positionTransformed].
 */
void SkinnedMesh3DAttributeTransformer::DestroyTransformedPositionFlagsArray()
{
	SAFE_DELETE(positionTransformed);
}

/*
 * Create the position transformation flag array [positionTransformed], and initialize
 * the save transformed position array [transformedPositions] to the appropriate length.
 */
bool SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray(unsigned int positionTransformedCount)
{
	this->positionTransformedCount = positionTransformedCount;
	positionTransformed = new unsigned char[positionTransformedCount];
	ASSERT(positionTransformed != NULL, "SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray -> Unable to allocate flags array.", false);

	bool initSuccess = transformedPositions.Init(positionTransformedCount);
	if(!initSuccess)
	{
		Debug::PrintError("SkinnedMesh3DAttributeTransformer::CreateTransformedPositionFlagsArray -> Could not init transformed vertex array.");
		return false;
	}

	return true;
}

/*
 * Clear the position transformation flag array [positionTransformed].
 */
void SkinnedMesh3DAttributeTransformer::ClearTransformedPositionFlagsArray()
{
	memset(positionTransformed, 0, sizeof(unsigned char) * positionTransformedCount);
}

/*
 * Destroy the normal transformation flag array [normalTransformed].
 */
void SkinnedMesh3DAttributeTransformer::DestroyTransformedNormalFlagsArray()
{
	SAFE_DELETE(normalTransformed);
}

/*
 * Create the normal transformation flag array [normalTransformed], and initialize
 * the save normal position array [transformedNormals] to the appropriate length.
 */
bool SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray(unsigned int normalTransformedCount)
{
	this->normalTransformedCount = normalTransformedCount;
	normalTransformed = new unsigned char[normalTransformedCount];
	ASSERT(normalTransformed != NULL, "SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray -> Unable to allocate flags array.", false);

	bool initSuccess = transformedNormals.Init(normalTransformedCount);
	ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateTransformedNormalFlagsArray -> Could not init transformed vertex array.", false);

	return true;
}

/*
 * Clear the normal transformation flag array [normalTransformed].
 */
void SkinnedMesh3DAttributeTransformer::ClearTransformedNormalFlagsArray()
{
	memset(normalTransformed, 0, sizeof(unsigned char) * normalTransformedCount);
}

/*
 * Set the target Skeleton object for this instance.
 */
void SkinnedMesh3DAttributeTransformer::SetSkeleton(SkeletonRef skeleton)
{
	this->skeleton = skeleton;
	DestroyTransformedBoneFlagsArray();
	CreateTransformedBoneFlagsArray();
}

/*
 * Set the index of this instance's VertexBoneMap in [skeleton].
 */
void SkinnedMesh3DAttributeTransformer::SetVertexBoneMapIndex(int index)
{
	vertexBoneMapIndex = index;
}

/*
 * Perform transformation (skinning) for both vertex positions and normals in a single function.
 *
 * [positionsIn] is a copy of the existing mesh positions, [positionsOut] is the array in which the transformed positions are placed.
 * [normalsIn] is a copy of the existing mesh normals, [normalsOut] is the array in which the transformed normals are placed.
 * [centerIn] is a copy of the existing center of the mesh, [centerOut] is where the transformed center is placed.
 */
void SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut,
																	 const Vector3Array& normalsIn, Vector3Array& normalsOut,
																	 const Point3& centerIn, Point3& centerOut)
{
	// copy existing positions to output array and the perform transformations
	// directly on output array
	positionsIn.CopyTo(&positionsOut);
	// copy existing normals to output array and the perform transformations
	// directly on output array
	normalsIn.CopyTo(&normalsOut);
	// copy existing center to output center and perform transformation
	// directly on output center
	centerOut.Set(centerIn.x,centerIn.y,centerIn.z);

	// number of unique bones encountered, used to calculate the average of the bone offsets
	unsigned int uniqueBonesEncountered = 0;
	// store the average bone offset, which will be used to transform the center point
	Matrix4x4 averageBoneOffset;

	// make sure the target skeleton is valid and has a VertexBoneMap object for this instance
	if(skeleton.IsValid() && vertexBoneMapIndex >= 0)
	{
		ClearTransformedBoneFlagsArray();

		Matrix4x4 temp;

		// used to store full transformation for a vertex
		Matrix4x4 full;

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = skeleton->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT_RTRN(vertexBoneMap != NULL,"SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> No valid vertex bone map found for sub mesh.");

		// get the number of unique vertices in the vertex bone map
		unsigned int uniqueVertexCount = vertexBoneMap->GetUVertexCount();

		// initialize the position transformation flags array, saved transformed position array,
		// normal transformation flags array, and saved transformed normal array
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

		// since there is a 1-to-1 correspondence between normals and arrays, we only need to use of of the
		// transformation flags arrays. in this case it will be the position transformation array
		ClearTransformedPositionFlagsArray();

		// loop through each vertex
		for(unsigned int i = 0; i < positionsOut.GetCount(); i++)
		{
			// get the mapping information for the current vertex
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);

			// If this vertex has already been visited, simply apply the saved transformations to the
			// current instance. The value of desc->UVertexIndex indicates the current vertex's unique
			// vertex value (multiple vertices in multiple triangles may actually be the same vertex, just
			// duplicated for each triangle)
			if(positionTransformed[desc->UVertexIndex] == 1)
			{
				Point3 * p = transformedPositions.GetPoint(desc->UVertexIndex);
				positionsOut.GetPoint(i)->Set(p->x,p->y,p->z);

				Vector3 * v = transformedNormals.GetVector(desc->UVertexIndex);
				normalsOut.GetVector(i)->Set(v->x,v->y,v->z);
			}
			else
			{
				// if not attached to any bones, then the transformation
				// matrix will be the identity matrix
				if(desc->BoneCount == 0)full.SetIdentity();

				// loop through each bone to which this vertex is attached and calculate the final
				// transformation for this vertex by applying the respective weight for each bone
				// transformation and adding them up.
				for(unsigned int b = 0; b < desc->BoneCount; b++)
				{
					Bone * bone = skeleton->GetBone(desc->BoneIndex[b]);

					// We need to calculate the transformation that is formed by the combination
					// of the current bone's offset matrix, and the full transformation of
					// the corresponding node in [skeleton]. If it has already been calculated,
					// which is indicated by the bone transformation flag array in [boneTransformed],
					// then we skip this step. A value of zero means it is not yet calculated.
					if(boneTransformed[desc->BoneIndex[b]] == 0)
					{
						bone->TempFullMatrix.SetTo(&bone->OffsetMatrix);

						if(bone->Node->HasTarget())
						{
							const Transform * targetFull = bone->Node->GetFullTransform();

							// store final transformation for this bone in bone->TempFullMatrix
							bone->TempFullMatrix.PreMultiply(targetFull->GetMatrix());
						}

						// factor into average bone offset
						averageBoneOffset.Add(&bone->OffsetMatrix);

						boneTransformed[desc->BoneIndex[b]] = 1;
						uniqueBonesEncountered++;
					}

					temp.SetTo(&bone->TempFullMatrix);
					// apply weight for this vertex & bone
					temp.MultiplyByScalar(desc->Weight[b]);
					if(b==0)full.SetTo(&temp);
					else full.Add(&temp);
				}

				// transform position
				Point3 * p = positionsOut.GetPoint(i);
				full.Transform(p);

				// transform nomral
				Vector3 * v = normalsOut.GetVector(i);
				full.Transform(v);

				// update saved positions
				transformedPositions.GetPoint(desc->UVertexIndex)->SetTo(p);
				// update saved normals
				transformedNormals.GetVector(desc->UVertexIndex)->Set(v->x,v->y,v->z);
				// set position transformation flag to 1 so we don't repeat the work for this vertex if we
				// encounter it again
				positionTransformed[desc->UVertexIndex] = 1;
			}
		}

		// calculate average bone offset
		if(uniqueBonesEncountered==0)uniqueBonesEncountered=1;
		averageBoneOffset.MultiplyByScalar(1/(float)uniqueBonesEncountered);
		// apply average bone offset to center point
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
		ASSERT_RTRN(vertexBoneMap != NULL,"SkinnedMesh3DAttributeTransformer::TransformPositions -> No valid vertex bone map found for sub mesh.");

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
