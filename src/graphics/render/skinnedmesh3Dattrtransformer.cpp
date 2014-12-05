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

	straightNormalTransformedCount = -1;
	straightNormalTransformed = NULL;

	savedTransformCount = -1;
	transformCalculated = NULL;
	savedTransforms = NULL;

	identicalNormalCount = -1;
	identicalNormals = NULL;
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

	straightNormalTransformedCount = -1;
	straightNormalTransformed = NULL;

	savedTransformCount = -1;
	transformCalculated = NULL;
	savedTransforms = NULL;

	identicalNormalCount = -1;
	identicalNormals = NULL;
}

/*
 * Cleanup.
 */
SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
{
	DestroyTransformedBoneFlagsArray();
	DestroyTransformCache(Target::Position);
	DestroyTransformCache(Target::Normal);
	DestroyTransformCache(Target::StraightNormal);
	DestroyTransformCache(Target::Transform);
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
 * Destroy the flag array specified by [target].
 */
void SkinnedMesh3DAttributeTransformer::DestroyTransformCache(Target target)
{
	if(target == Target::Position)
	{
		SAFE_DELETE(positionTransformed);
	}
	else if(target == Target::Normal)
	{
		SAFE_DELETE(normalTransformed);
	}
	else if(target == Target::StraightNormal)
	{
		SAFE_DELETE(straightNormalTransformed);
	}
	else if(target == Target::IdenticalNormal)
	{
		SAFE_DELETE(identicalNormals);
	}
	else if(target == Target::Transform)
	{
		if(savedTransforms != NULL)
		{
			delete[] savedTransforms;
			savedTransforms = NULL;
		}
		SAFE_DELETE(transformCalculated);
	}
}

/*
 * Create the flag array specified by [target], and initialize
 * the saved/transformed data array to the appropriate length.
 */
bool SkinnedMesh3DAttributeTransformer::CreateTransformCache(Target target, unsigned int count)
{
	if(target == Target::Position)
	{
		positionTransformedCount = count;
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
	else if(target == Target::Normal)
	{
		normalTransformedCount = count;
		normalTransformed = new unsigned char[normalTransformedCount];
		ASSERT(normalTransformed != NULL, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Unable to allocate normal flags array.", false);

		bool initSuccess = transformedNormals.Init(normalTransformedCount);
		ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Could not init transformed normal array.", false);

		return true;
	}
	else if(target == Target::StraightNormal)
	{
		straightNormalTransformedCount = count;
		straightNormalTransformed = new unsigned char[straightNormalTransformedCount];
		ASSERT(straightNormalTransformed != NULL, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Unable to allocate straight normal flags array.", false);

		bool initSuccess = transformedStraightNormals.Init(straightNormalTransformedCount);
		ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Could not init transformed straight normal array.", false);

		return true;
	}
	else if(target == Target::IdenticalNormal)
	{
		identicalNormalCount = count;
		identicalNormals = new unsigned char[identicalNormalCount];
		ASSERT(identicalNormals != NULL, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Unable to allocate identicalNormals flags array.", false);

		return true;
	}
	else if(target == Target::Transform)
	{
		savedTransformCount = count;
		transformCalculated = new unsigned char[savedTransformCount];
		ASSERT(transformCalculated != NULL, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Unable to allocate saved transform flags array.", false);

		savedTransforms = new Matrix4x4[savedTransformCount];
		ASSERT(savedTransforms != NULL, "SkinnedMesh3DAttributeTransformer::CreateFlagsArray -> Could not saved transform array.", false);

		return true;
	}

	return false;
}

/*
 * Clear the flag array specified by [target].
 */
void SkinnedMesh3DAttributeTransformer::ClearTransformCache(Target target)
{
	SetAllTransformCache(target, 0);
}

/*
 * Set all the flags in the array specified by [target] to be [value].
 */
void SkinnedMesh3DAttributeTransformer::SetAllTransformCache(Target target, unsigned char value)
{
	if(target == Target::Position)
	{
		memset(positionTransformed, value, sizeof(unsigned char) * positionTransformedCount);
	}
	else if(target == Target::Normal)
	{
		memset(normalTransformed, value, sizeof(unsigned char) * normalTransformedCount);
	}
	else if(target == Target::StraightNormal)
	{
		memset(straightNormalTransformed, value, sizeof(unsigned char) * straightNormalTransformedCount);
	}
	else if(target == Target::StraightNormal)
	{
		memset(identicalNormals, value, sizeof(unsigned char) * identicalNormalCount);
	}
	else if(target == Target::Transform)
	{
		memset(transformCalculated, value, sizeof(unsigned char) * savedTransformCount);
	}
}

/*
 * For each unique vertex, look at the normal for each instance of that vertex. If all those
 * normals are equal (e.g. all averaged for smoothing), store 1 in [identicalNormals] for the
 * unique vertex; otherwise store 0.
 *
 * The values in [identicalNormals] are used in the attribute transformation functions to determine:
 *
 * if the normals for all instances of a unique vertex are the same, and therefore can
 * have the transformation calculation performed once and the result applied to each instance.
 *
 *   -or-
 *
 * if there are non-equivalent normals among the instances of a unique vertex (e.g. in the case
 * where angles between faces are too sharp for smoothed normals and therefore the transformation
 * for each instance must be calculated individually.
 */
void SkinnedMesh3DAttributeTransformer::FindIdenticalNormals(Vector3Array& fullNormalList)
{
	// retrieve this instance's vertex bone map
	VertexBoneMap * vertexBoneMap = skeleton->GetVertexBoneMap(vertexBoneMapIndex);
	ASSERT_RTRN(vertexBoneMap != NULL,"SkinnedMesh3DAttributeTransformer::FindIdenticalNormals -> No valid vertex bone map found for sub mesh.");

	unsigned int uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();
	ASSERT_RTRN(fullNormalList.GetMaxCount() == vertexBoneMap->GetVertexCount(),
				"SkinnedMesh3DAttributeTransformer::FindIdenticalNormals -> fullNormalList vertex count does not match vertex bone map.");

	DestroyTransformCache(Target::IdenticalNormal);
	bool createSuccess = CreateTransformCache(Target::IdenticalNormal, uniqueVertexCount);
	ASSERT_RTRN(createSuccess == true, "SkinnedMesh3DAttributeTransformer::FindIdenticalNormals -> Unable to create identical normals array.");

	Vector3Array seenNormalValues;
	std::vector<bool> seenNormals;

	bool initSeenNormals = seenNormalValues.Init(uniqueVertexCount);
	ASSERT_RTRN(initSeenNormals != false,"SkinnedMesh3DAttributeTransformer::FindIdenticalNormals -> Unable to init seenNormalValues.");

	for(unsigned int i =0; i < uniqueVertexCount; i++)
	{
		seenNormals.push_back(false);
		identicalNormals[i] = 1;
	}

	VertexBoneMap::VertexMappingDescriptor * desc;
	unsigned int fullNormalListSize = fullNormalList.GetMaxCount();
	for(unsigned int i = 0; i < fullNormalListSize; i++)
	{
		desc = vertexBoneMap->GetDescriptor(i);
		if(desc == NULL)continue;

		if(!seenNormals[desc->UniqueVertexIndex])
		{
			seenNormalValues.GetVector(desc->UniqueVertexIndex)->SetTo(*(fullNormalList.GetVector(i)));
			seenNormals[desc->UniqueVertexIndex] = true;
		}
		else
		{
			if(!Vector3::AreStrictlyEqual(seenNormalValues.GetVector(desc->UniqueVertexIndex), fullNormalList.GetVector(i)))
			{
				identicalNormals[desc->UniqueVertexIndex] = 0;
			}
		}
	}
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
																	 const Vector3Array& straightNormalsIn, Vector3Array& straightNormalsOut,
																	 const Point3& centerIn, Point3& centerOut)
{
	// copy existing positions to output array and the perform transformations
	// directly on output array
	positionsIn.CopyTo(&positionsOut);
	// copy existing normals to output array and the perform transformations
	// directly on output array
	normalsIn.CopyTo(&normalsOut);
	// copy existing straight normals to output array and the perform transformations
	// directly on output array
	straightNormalsIn.CopyTo(&straightNormalsOut);
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
		unsigned int uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();

		// initialize the position transformation flags array, saved transformed position array,
		// normal transformation flags array, and saved transformed normal array
		if(positionTransformedCount < 0 || uniqueVertexCount != (unsigned int)positionTransformedCount)
		{
			DestroyTransformCache(Target::Position);
			bool createSuccess = CreateTransformCache(Target::Position, uniqueVertexCount);
			ASSERT_RTRN(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create position transform flags array.");

			DestroyTransformCache(Target::Normal);
			createSuccess = CreateTransformCache(Target::Normal, uniqueVertexCount);
			ASSERT_RTRN(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create normal transform flags array.");

			DestroyTransformCache(Target::StraightNormal);
			createSuccess = CreateTransformCache(Target::StraightNormal, uniqueVertexCount);
			ASSERT_RTRN(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create straight normal transform flags array.");

			DestroyTransformCache(Target::Transform);
			createSuccess = CreateTransformCache(Target::Transform, uniqueVertexCount);
			ASSERT_RTRN(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> Unable to create saved transforms array.");

			FindIdenticalNormals(normalsOut);
		}

		// since there is a 1-to-1 correspondence between normals and positions and transforms, we only need to use of of the
		// transformation flags arrays. in this case it will be the position transformation array
		ClearTransformCache(Target::Position);

		float* transformedPositionsPtrBase =  const_cast<float*>(transformedPositions.GetDataPtr());
		float* transformedNormalsPtrBase = const_cast<float*>(transformedNormals.GetDataPtr());
		float* transformedStraightNormalsPtrBase = const_cast<float*>(transformedStraightNormals.GetDataPtr());
		float* transformedPositionsPtr = NULL;
		float* transformedNormalsPtr = NULL;
		float* transformedStraightNormalsPtr = NULL;

		float * positionsOutBase = const_cast<float*>(positionsOut.GetDataPtr());
		float * normalsOutBase = const_cast<float*>(normalsOut.GetDataPtr());
		float * straightNormalsOutBase = const_cast<float*>(straightNormalsOut.GetDataPtr());

		unsigned int fullPositionCount = positionsOut.GetUsedCount();
		// loop through each vertex
		for(unsigned int i = 0; i < fullPositionCount; i++)
		{
			float * currentPositionPtr = positionsOutBase + (i*4);//positionsIteratorCurrent.GetDataPtr();
			float * currentNormalPtr =  normalsOutBase + (i*4); //normalsIteratorCurrent.GetDataPtr();
			float * currentStraightNormalPtr =  straightNormalsOutBase + (i*4); //straightNormalsIteratorCurrent.GetDataPtr();

			// get the mapping information for the current vertex
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);

			// If this vertex has already been visited, simply apply the saved transformations to the
			// current instance. The value of desc->UVertexIndex indicates the current vertex's unique
			// vertex value (multiple vertices in multiple triangles may actually be the same vertex, just
			// duplicated for each triangle)
			if(positionTransformed[desc->UniqueVertexIndex] == 1)
			{
				transformedPositionsPtr = transformedPositionsPtrBase+(desc->UniqueVertexIndex*4);
				BaseVector4_QuickCopy(transformedPositionsPtr, currentPositionPtr);

				transformedNormalsPtr = transformedNormalsPtrBase+(desc->UniqueVertexIndex*4);
				if(identicalNormals[desc->UniqueVertexIndex])
				{
					BaseVector4_QuickCopy(transformedNormalsPtr, currentNormalPtr);
				}
				else savedTransforms[desc->UniqueVertexIndex].Transform(currentNormalPtr);

				transformedStraightNormalsPtr = transformedStraightNormalsPtrBase+(desc->UniqueVertexIndex*4);
				BaseVector4_QuickCopy(transformedStraightNormalsPtr, currentStraightNormalPtr);
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
						bone->TempFullMatrix.SetTo(bone->OffsetMatrix);

						if(bone->Node->HasTarget())
						{
							const Transform * targetFull = bone->Node->GetFullTransform();
							targetFull->CopyMatrix(temp);

							// store final transformation for this bone in bone->TempFullMatrix
							bone->TempFullMatrix.PreMultiply(temp);
							bone->TempFullMatrix.PreMultiply(modelInverse);
						}

						// factor into average bone offset
						averageBoneOffset.Add(bone->OffsetMatrix);

						boneTransformed[desc->BoneIndex[b]] = 1;
						uniqueBonesEncountered++;
					}

					temp.SetTo(bone->TempFullMatrix);
					// apply weight for this vertex & bone
					temp.MultiplyByScalar(desc->Weight[b]);

					if(b==0)full.SetTo(temp);
					else full.Add(temp);
				}

				full.Transform(currentPositionPtr);
				full.Transform(currentNormalPtr);
				full.Transform(currentStraightNormalPtr);

				// update saved positions & normals
				transformedPositionsPtr = transformedPositionsPtrBase+(desc->UniqueVertexIndex*4);
				transformedNormalsPtr = transformedNormalsPtrBase+(desc->UniqueVertexIndex*4);
				transformedStraightNormalsPtr = transformedStraightNormalsPtrBase+(desc->UniqueVertexIndex*4);

				BaseVector4_QuickCopy(currentNormalPtr, transformedNormalsPtr);
				BaseVector4_QuickCopy(currentPositionPtr, transformedPositionsPtr);
				BaseVector4_QuickCopy(currentStraightNormalPtr, transformedStraightNormalsPtr);

				savedTransforms[desc->UniqueVertexIndex] = full;

				// set position transformation flag to 1 so we don't repeat the work for this vertex if we
				// encounter it again
				positionTransformed[desc->UniqueVertexIndex] = 1;
			}
		}

		// calculate average bone offset
		if(uniqueBonesEncountered==0)uniqueBonesEncountered=1;
		averageBoneOffset.MultiplyByScalar(1/(float)uniqueBonesEncountered);
		// apply average bone offset to center point
		averageBoneOffset.Transform(centerOut);
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

		unsigned int uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();
		if(positionTransformedCount < 0 || uniqueVertexCount != (unsigned int)positionTransformedCount)
		{
			DestroyTransformCache(Target::Position);
			bool createSuccess = CreateTransformCache(Target::Position, uniqueVertexCount);

			if(!createSuccess)
			{
				Debug::PrintError("SkinnedMesh3DAttributeTransformer::TransformPositions -> Unable to create position transform flags array.");
				return;
			}
		}

		ClearTransformCache(Target::Position);

		for(unsigned int i = 0; i < positionsOut.GetUsedCount(); i++)
		{
			VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);
			if(positionTransformed[desc->UniqueVertexIndex] == 1)
			{
				Point3 * p = transformedPositions.GetPoint(desc->UniqueVertexIndex);
				ASSERT_RTRN(p!=NULL,"SkinnedMesh3DAttributeTransformer::TransformPositionsAndNormals -> transformedPositions contains NULL point.");
				positionsOut.GetPoint(i)->SetTo(*p);
			}
			else
			{
				if(desc->BoneCount == 0)full.SetIdentity();
				for(unsigned int b = 0; b < desc->BoneCount; b++)
				{
					Bone * bone = skeleton->GetBone(desc->BoneIndex[b]);

					if(boneTransformed[desc->BoneIndex[b]] == 0)
					{
						bone->TempFullMatrix.SetTo(bone->OffsetMatrix);

						if(bone->Node->HasTarget())
						{
							const Transform * targetFull = bone->Node->GetFullTransform();
							targetFull->CopyMatrix(temp);

							bone->TempFullMatrix.PreMultiply(temp);
						}

						averageBoneOffset.Add(bone->OffsetMatrix);
						boneTransformed[desc->BoneIndex[b]] = 1;
					}

					temp.SetTo(bone->TempFullMatrix);
					temp.MultiplyByScalar(desc->Weight[b]);
					if(b==0)full.SetTo(temp);
					else full.Add(temp);
				}

				Point3 * p = positionsOut.GetPoint(i);
				ASSERT_RTRN(p!=NULL,"SkinnedMesh3DAttributeTransformer::TransformPositions -> positionsOut contains NULL point.");
				full.Transform(*p);

				transformedPositions.GetPoint(desc->UniqueVertexIndex)->SetTo(*p);
				positionTransformed[desc->UniqueVertexIndex] = 1;
			}
		}

		if(uniqueBonesEncountered==0)uniqueBonesEncountered=1;
		averageBoneOffset.MultiplyByScalar(1/(float)uniqueBonesEncountered);
		averageBoneOffset.Transform(centerOut);
	}
}

void SkinnedMesh3DAttributeTransformer::TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut,
														 const Vector3Array& straightNormalsIn, Vector3Array& straightNormalsOut)
{
	normalsIn.CopyTo(&normalsOut);
	straightNormalsIn.CopyTo(&straightNormalsOut);
}
