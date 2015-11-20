#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "skinnedmesh3Drenderer.h"
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
#include "scene/sceneobject.h"
#include "global/assert.h"
#include "util/time.h"

namespace GTE
{
	/*
	* Default constructor, initialize all member variables to default values.
	*/
	SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer() : AttributeTransformer()
	{
		boneCount = -1;
		vertexBoneMapIndex = -1;
		currentCacheSize = -1;

		boneTransformed = nullptr;
		savedTransforms = nullptr;
		identicalNormalFlags = nullptr;
		identicalTangentFlags = nullptr;

		cacheFlags = nullptr;

		renderer = nullptr;
	}

	/*
	 * Parameterized constructor - indicated which mesh attributes will be transformed via [attributes].
	 */
	SkinnedMesh3DAttributeTransformer::SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes) : AttributeTransformer(attributes)
	{
		boneCount = -1;
		vertexBoneMapIndex = -1;
		currentCacheSize = -1;

		boneTransformed = nullptr;
		savedTransforms = nullptr;
		identicalNormalFlags = nullptr;
		identicalTangentFlags = nullptr;

		cacheFlags = nullptr;

		renderer = nullptr;
	}

	/*
	 * Cleanup.
	 */
	SkinnedMesh3DAttributeTransformer::~SkinnedMesh3DAttributeTransformer()
	{
		DestroyTransformedBoneFlagsArray();
		DestroyCaches();
		DestroyIdenticalNormalsTangentsFlags();
	}

	/*
	 * Update the size of the transformed bone flags array to match the
	 * skeleton in this transofrmer's renderer.
	 */
	void SkinnedMesh3DAttributeTransformer::UpdateTransformedBoneCacheSize()
	{
		DestroyTransformedBoneFlagsArray();
		CreateTransformedBoneFlagsArray();
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
	Bool SkinnedMesh3DAttributeTransformer::CreateTransformedBoneFlagsArray()
	{
		if (renderer != nullptr && renderer->GetSkeleton().IsValid())
		{
			SkeletonRef skeleton = renderer->GetSkeleton();
			boneCount = skeleton->GetBoneCount();
			boneTransformed = new(std::nothrow) UChar[boneCount];
			ASSERT(boneTransformed != nullptr, "SkinnedMesh3DAttributeTransformer::CreateTransformedBoneFlagsArray -> Unable to allocate flags array.");
			return true;
		}

		return false;
	}

	/*
	 * Clear the bone transformation array.
	 */
	void SkinnedMesh3DAttributeTransformer::ClearTransformedBoneFlagsArray()
	{
		if (renderer != nullptr && renderer->GetSkeleton().IsValid())
		{
			SkeletonRef skeleton = renderer->GetSkeleton();
			memset(boneTransformed, 0, sizeof(UChar) * skeleton->GetBoneCount());
		}
	}

	/*
	 * Destroy the flag array specified by [target].
	 */
	void SkinnedMesh3DAttributeTransformer::DestroyCache(CacheType target)
	{
		if (target == CacheType::Position)
		{

		}
		else if (target == CacheType::VertexNormal)
		{

		}
		else if (target == CacheType::FaceNormal)
		{

		}
		else if (target == CacheType::VertexTangent)
		{

		}
		else if (target == CacheType::Transform)
		{
			if (savedTransforms != nullptr)
			{
				delete[] savedTransforms;
				savedTransforms = nullptr;
			}
		}
	}

	/*
	 * Create the flag array specified by [target], and initialize
	 * the saved/transformed data array to the appropriate length.
	 */
	Bool SkinnedMesh3DAttributeTransformer::CreateCache(CacheType target)
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCache -> Renderer is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCache -> No valid vertex bone map found for sub mesh.");

		UInt32 count = vertexBoneMap->GetUniqueVertexCount();

		if (target == CacheType::Position)
		{
			Bool initSuccess = transformedPositions.Init(count);
			if (!initSuccess)
			{
				Debug::PrintError("SkinnedMesh3DAttributeTransformer::CreateCache -> Could not init transformed vertex array.");
				return false;
			}

			return true;
		}
		else if (target == CacheType::VertexNormal)
		{
			Bool initSuccess = transformedVertexNormals.Init(count);
			ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateCache -> Could not init transformed vertex normal array.");


			return true;
		}
		else if (target == CacheType::FaceNormal)
		{
			Bool initSuccess = transformedFaceNormals.Init(count);
			ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateCache -> Could not init transformed face normal array.");

			return true;
		}
		else if (target == CacheType::VertexTangent)
		{
			Bool initSuccess = transformedVertexTangents.Init(count);
			ASSERT(initSuccess, "SkinnedMesh3DAttributeTransformer::CreateCache -> Could not init transformed vertex tangent array.");


			return true;
		}
		else if (target == CacheType::Transform)
		{
			savedTransforms = new(std::nothrow) Matrix4x4[count];
			ASSERT(savedTransforms != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCache -> Could not saved transform array.");

			return true;
		}

		return false;
	}

	/*
	 * Clear the flag array specified by [target].
	 */
	void SkinnedMesh3DAttributeTransformer::ClearCacheFlags()
	{
		SetAllTransformCacheFlags(0);
	}

	/*
	 * Set all the flags in the array specified by [target] to be [value].
	 */
	void SkinnedMesh3DAttributeTransformer::SetAllTransformCacheFlags(UChar value)
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::SetAllTransformCacheFlags -> Renderer is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::SetAllTransformCacheFlags -> No valid vertex bone map found for sub mesh.");

		UInt32 count = vertexBoneMap->GetUniqueVertexCount();

		memset(cacheFlags, value, sizeof(UChar) * count);
	}

	/*
	 * Destroy the identical normal flags array and identical tangent flags array.
	 */
	void SkinnedMesh3DAttributeTransformer::DestroyIdenticalNormalsTangentsFlags()
	{
		SAFE_DELETE(identicalNormalFlags);
		SAFE_DELETE(identicalTangentFlags);
	}

	/*
	 * Create the identical normal flags array and identical tangent flags array.
	 */
	Bool SkinnedMesh3DAttributeTransformer::CreateIdenticalNormalsTangentsFlags()
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::CreateIdenticalNormalsTangentsFlags -> 'renderer' is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::CreateIdenticalNormalsTangentsFlags -> No valid vertex bone map found for sub mesh.");

		UInt32 count = vertexBoneMap->GetUniqueVertexCount();

		identicalNormalFlags = new(std::nothrow) UChar[count];
		ASSERT(identicalNormalFlags != nullptr, "SkinnedMesh3DAttributeTransformer::CreateIdenticalNormalsTangentsFlags -> Unable to allocate identical normal flags array.");

		identicalTangentFlags = new(std::nothrow) UChar[count];
		ASSERT(identicalTangentFlags != nullptr, "SkinnedMesh3DAttributeTransformer::CreateIdenticalNormalsTangentsFlags -> Unable to allocate identical tangent flags array.");

		return true;
	}

	/*
	 * Clear the identical normal flags array and identical tangent flags array.
	 */
	void SkinnedMesh3DAttributeTransformer::ClearIdenticalNormalsTangentsFlags()
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::ClearIdenticalNormalsFlags -> 'renderer' is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::ClearIdenticalNormalsTangentsFlags -> No valid vertex bone map found for sub mesh.");

		UInt32 count = vertexBoneMap->GetUniqueVertexCount();

		memset(identicalNormalFlags, 0, sizeof(UChar) * count);
		memset(identicalTangentFlags, 0, sizeof(UChar) * count);
	}


	/*
	 * For each unique vertex, look at one of: [normal, tangent] for each instance of that vertex. If all the
	 * vectors are equal (e.g. all averaged for smoothing), store 1 in one of[identicalNormalFlags, identicalTangentFlags] for the
	 * unique vertex; otherwise store 0.
	 *
	 * The values in [identicalNormalFlags] and [identicalTangentFlags] are used in the attribute transformation functions to determine:
	 *
	 * if the [[normals, tangents] for all instances of a unique vertex are the same, and therefore can
	 * have the transformation calculation performed once and the result applied to each instance.
	 *
	 *   - or -
	 *
	 * if there are non-equivalent [normals, tangents] among the instances of a unique vertex (e.g. in the case
	 * where angles between faces are too sharp for smoothing and therefore the transformation
	 * for each instance must be calculated individually).
	 */
	Bool SkinnedMesh3DAttributeTransformer::FindIdenticalNormalsOrTangents(Vector3Array& fullList, Bool forNormals)
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::FindIdenticalNormals -> renderer is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::FindIdenticalNormalsOrTangents -> No valid vertex bone map found for sub mesh.");

		UInt32 uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();
		ASSERT(fullList.GetCount() == vertexBoneMap->GetVertexCount(),
			"SkinnedMesh3DAttributeTransformer::FindIdenticalNormalsOrTangents -> 'fullList' vertex count does not match vertex bone map.");

		Vector3Array seenVectorValues;
		std::vector<Bool> seenVectors;

		Bool initSeenVectorValues = seenVectorValues.Init(uniqueVertexCount);
		ASSERT(initSeenVectorValues != false, "SkinnedMesh3DAttributeTransformer::FindIdenticalNormalsOrTangents -> Unable to init seenVectorValues.");

		for (UInt32 i = 0; i < uniqueVertexCount; i++)
		{
			seenVectors.push_back(false);
			if (forNormals)identicalNormalFlags[i] = 1;
			else identicalTangentFlags[i] = 1;
		}

		VertexBoneMap::VertexMappingDescriptor * desc;
		UInt32 fullListSize = fullList.GetCount();
		for (UInt32 i = 0; i < fullListSize; i++)
		{
			desc = vertexBoneMap->GetDescriptor(i);
			if (desc == nullptr)continue;

			if (!seenVectors[desc->UniqueVertexIndex])
			{
				seenVectorValues.GetVector(desc->UniqueVertexIndex)->SetTo(*(fullList.GetVector(i)));
				seenVectors[desc->UniqueVertexIndex] = true;
			}
			else
			{
				if (!Vector3::AreStrictlyEqual(seenVectorValues.GetVector(desc->UniqueVertexIndex), fullList.GetVector(i)))
				{
					if (forNormals)identicalNormalFlags[desc->UniqueVertexIndex] = 0;
					else identicalTangentFlags[desc->UniqueVertexIndex] = 0;
				}
			}
		}

		return true;
	}

	/*
	 * Create all the caches needed by the TransformXXX() methods. This means create and initialize a cache for:
	 *
	 *   1. Transformed bones
	 *   2. Transformed vertex positions
	 *   3. Transformed vertex normals
	 *   4. Transformed face normals
	 *   5. Transformed vertex tangents
	 *   6. Per-vertex Transforms
	 *
	 *  This method also initializes the [cacheFlags] array, which is used to flag which entries in all the other caches are valid.
	 */
	Bool SkinnedMesh3DAttributeTransformer::CreateCaches()
	{
		ASSERT(renderer != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCaches -> 'renderer' is null.");

		// retrieve this instance's vertex bone map
		VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
		ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCaches -> No valid vertex bone map found for sub mesh.");

		UInt32 uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();

		cacheFlags = new(std::nothrow) UChar[uniqueVertexCount];
		ASSERT(cacheFlags != nullptr, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to allocate cacheFlags flags array.");

		DestroyCache(CacheType::Position);
		Bool createSuccess = CreateCache(CacheType::Position);
		ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to create position transform flags array.");

		DestroyCache(CacheType::VertexNormal);
		createSuccess = CreateCache(CacheType::VertexNormal);
		ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to create vertex normal transform flags array.");

		DestroyCache(CacheType::FaceNormal);
		createSuccess = CreateCache(CacheType::FaceNormal);
		ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to create face normal transform flags array.");

		DestroyCache(CacheType::VertexTangent);
		createSuccess = CreateCache(CacheType::VertexTangent);
		ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to create vertex tangent transform flags array.");


		DestroyCache(CacheType::Transform);
		createSuccess = CreateCache(CacheType::Transform);
		ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::CreateCaches -> Unable to create saved transforms array.");

		return true;
	}

	/*
	 * Deallocate & destroy all of the transformation caches.
	 */
	void SkinnedMesh3DAttributeTransformer::DestroyCaches()
	{
		SAFE_DELETE(cacheFlags);
		DestroyCache(CacheType::Position);
		DestroyCache(CacheType::VertexNormal);
		DestroyCache(CacheType::FaceNormal);
		DestroyCache(CacheType::VertexTangent);
		DestroyCache(CacheType::Transform);
	}

	/*
	 * Set the target Skeleton object for this instance.
	 */
	void SkinnedMesh3DAttributeTransformer::SetRenderer(SkinnedMesh3DRenderer* renderer)
	{
		this->renderer = renderer;
	}

	/*
	 * Set the index of this instance's VertexBoneMap in [skeleton].
	 */
	void SkinnedMesh3DAttributeTransformer::SetVertexBoneMapIndex(Int32 index)
	{
		vertexBoneMapIndex = index;
	}

	/*
	 * Perform transformation (skinning) for both vertex positions and normals in a single function.
	 *
	 * [positionsIn] is a copy of the existing mesh positions, [positionsOut] is the array in which the transformed positions are placed.
	 * [vertexNormalsIn] is a copy of the existing mesh vertex normals, [vertexNormalsOut] is the array in which the transformed vertex normals are placed.
	 * [faceNormalsIn] is a copy of the existing mesh face normals, [faceNormalsOut] is the array in which the transformed face normals are placed.
	 * [centerIn] is a copy of the existing center of the mesh, [centerOut] is where the transformed center is placed.
	 */
	void SkinnedMesh3DAttributeTransformer::TransformAttributes(const Point3Array& positionsIn, Point3Array& positionsOut,
		const Vector3Array& vertexNormalsIn, Vector3Array& vertexNormalsOut,
		const Vector3Array& faceNormalsIn, Vector3Array& faceNormalsOut,
		const Vector3Array& vertexTangentsIn, Vector3Array& vertexTangentsOut,
		const Point3& centerIn, Point3& centerOut,
		Bool transformPositions, Bool transformNormals, Bool transformTangents)
	{
		// make sure the target skeleton is valid and has a VertexBoneMap object for this instance
		if (renderer != nullptr && vertexBoneMapIndex >= 0)
		{
			// copy existing positions to output array and the perform transformations
			// directly on output array
			if (transformPositions)positionsIn.CopyTo(&positionsOut);
			// copy existing vertex normals to output array and the perform transformations
			// directly on output array
			if (transformNormals)vertexNormalsIn.CopyTo(&vertexNormalsOut);
			// copy existing face normals to output array and the perform transformations
			// directly on output array
			if (transformNormals)faceNormalsIn.CopyTo(&faceNormalsOut);
			// copy existing vertex tangents to output array and the perform transformations
			// directly on output array
			if (transformTangents)vertexTangentsIn.CopyTo(&vertexTangentsOut);
			// copy existing center to output center and perform transformation
			// directly on output center
			centerOut.Set(centerIn.x, centerIn.y, centerIn.z);

			// number of unique bones encountered, used to calculate the average of the bone offsets
			UInt32 uniqueBonesEncountered = 0;
			// store the average bone offset, which will be used to transform the center point
			Matrix4x4 averageBoneOffset;

			Matrix4x4 temp;
			// used to store full transformation for a vertex
			Matrix4x4 full;

			// retrieve this instance's vertex bone map
			VertexBoneMap * vertexBoneMap = renderer->GetVertexBoneMap(vertexBoneMapIndex);
			ASSERT(vertexBoneMap != nullptr, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> No valid vertex bone map found for sub mesh.");

			// get the number of unique vertices in the vertex bone map
			UInt32 uniqueVertexCount = vertexBoneMap->GetUniqueVertexCount();
			// get total number of vertices (including multiple instances of a unique vertex)
			UInt32 fullVertexCount = vertexBoneMap->GetVertexCount();

			if (transformPositions)ASSERT(positionsOut.GetCount() == fullVertexCount, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Mismatched position count.");
			if (transformNormals)ASSERT(vertexNormalsOut.GetCount() == fullVertexCount, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Mismatched vertex normal count.");
			if (transformNormals)ASSERT(faceNormalsOut.GetCount() == fullVertexCount, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Mismatched face normal count.")
				if (transformTangents)ASSERT(vertexTangentsOut.GetCount() == fullVertexCount, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Mismatched vertex tangent count.")

					// initialize the position transformation flags array, saved transformed position array,
					// normal transformation flags array, and saved transformed normal array
					if (currentCacheSize < 0 || uniqueVertexCount != (UInt32)currentCacheSize)
					{
						DestroyCaches();
						Bool createSuccess = CreateCaches();
						ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Unable to create caches.");

						DestroyIdenticalNormalsTangentsFlags();
						createSuccess = CreateIdenticalNormalsTangentsFlags();
						ASSERT(createSuccess == true, "SkinnedMesh3DAttributeTransformer::TransformAttributes -> Unable to create identical normal caches.");

						if (transformNormals)FindIdenticalNormalsOrTangents(vertexNormalsOut, true);
						if (transformTangents)FindIdenticalNormalsOrTangents(vertexTangentsOut, false);

						currentCacheSize = uniqueVertexCount;
					}

			SkeletonRef skeleton = renderer->GetSkeleton();
			ASSERT(skeleton.IsValid(), "SkinnedMesh3DAttributeTransformer::TransformAttributes -> renderer's skeleton is not valid.");

			if (boneCount < 0 || (UInt32)boneCount != skeleton->GetBoneCount())
			{
				UpdateTransformedBoneCacheSize();
			}

			ClearTransformedBoneFlagsArray();
			// clear the flags for all caches
			ClearCacheFlags();

			Real* transformedPositionsPtrBase = const_cast<Real*>(transformedPositions.GetDataPtr());
			Real* transformedVertexNormalsPtrBase = const_cast<Real*>(transformedVertexNormals.GetDataPtr());
			Real* transformedFaceNormalsPtrBase = const_cast<Real*>(transformedFaceNormals.GetDataPtr());
			Real* transformedVertexTangentsPtrBase = const_cast<Real*>(transformedVertexTangents.GetDataPtr());
			Real* transformedPositionsPtr = nullptr;
			Real* transformedVertexNormalsPtr = nullptr;
			Real* transformedFaceNormalsPtr = nullptr;
			Real* transformedVertexTangentsPtr = nullptr;

			Real * positionsOutBase = const_cast<Real*>(positionsOut.GetDataPtr());
			Real * vertexNormalsOutBase = const_cast<Real*>(vertexNormalsOut.GetDataPtr());
			Real * faceNormalsOutBase = const_cast<Real*>(faceNormalsOut.GetDataPtr());
			Real * vertexTangentsOutBase = const_cast<Real*>(vertexTangentsOut.GetDataPtr());

			UInt32 fullPositionCount = vertexBoneMap->GetVertexCount();
			// loop through each vertex
			for (UInt32 i = 0; i < fullPositionCount; i++)
			{
				Real * currentPositionPtr = positionsOutBase + (i * 4);
				Real * currentVertexNormalPtr = vertexNormalsOutBase + (i * 4);
				Real * currentFaceNormalPtr = faceNormalsOutBase + (i * 4);
				Real * currentVertexTangentPtr = vertexTangentsOutBase + (i * 4);

				// get the mapping information for the current vertex
				VertexBoneMap::VertexMappingDescriptor *desc = vertexBoneMap->GetDescriptor(i);

				// If this vertex has already been visited, simply apply the saved transformations to the
				// current instance. The value of desc->UVertexIndex indicates the current vertex's unique
				// vertex value (multiple vertices in multiple triangles may actually be the same vertex, just
				// duplicated for each triangle)

				if (cacheFlags[desc->UniqueVertexIndex] == 1)
				{
					if (transformPositions)
					{
						transformedPositionsPtr = transformedPositionsPtrBase + (desc->UniqueVertexIndex * 4);
						BaseVector4_QuickCopy(transformedPositionsPtr, currentPositionPtr);
						//savedTransforms[desc->UniqueVertexIndex].Transform(currentPositionPtr);
					}

					if (transformNormals)
					{
						transformedVertexNormalsPtr = transformedVertexNormalsPtrBase + (desc->UniqueVertexIndex * 4);
						if (identicalNormalFlags[desc->UniqueVertexIndex])
						{
							BaseVector4_QuickCopy(transformedVertexNormalsPtr, currentVertexNormalPtr);
						}
						else savedTransforms[desc->UniqueVertexIndex].Transform(currentVertexNormalPtr);

						transformedFaceNormalsPtr = transformedFaceNormalsPtrBase + (desc->UniqueVertexIndex * 4);
					}

					if (transformTangents)
					{
						transformedVertexTangentsPtr = transformedVertexTangentsPtrBase + (desc->UniqueVertexIndex * 4);
						if (identicalTangentFlags[desc->UniqueVertexIndex])
						{
							BaseVector4_QuickCopy(transformedVertexTangentsPtr, currentVertexTangentPtr);
						}
						else savedTransforms[desc->UniqueVertexIndex].Transform(currentVertexTangentPtr);
					}

					if (i % 3 == 0)savedTransforms[desc->UniqueVertexIndex].Transform(currentFaceNormalPtr);
				}
				else
				{
					// if not attached to any bones, then the transformation
					// matrix will be the identity matrix
					if (desc->BoneCount == 0)full.SetIdentity();

					// loop through each bone to which this vertex is attached and calculate the final
					// transformation for this vertex by applying the respective weight for each bone
					// transformation and adding them up.
					for (UInt32 b = 0; b < desc->BoneCount; b++)
					{
						Bone * bone = skeleton->GetBone(desc->BoneIndex[b]);

						// We need to calculate the transformation that is formed by the combination
						// of the current bone's offset matrix, and the full transformation of
						// the corresponding node in [skeleton]. If it has already been calculated,
						// which is indicated by the bone transformation flag array in [boneTransformed],
						// then we skip this step. A value of zero means it is not yet calculated.
						if (boneTransformed[desc->BoneIndex[b]] == 0)
						{
							bone->TempFullMatrix.SetTo(bone->OffsetMatrix);

							if (bone->Node->HasTarget())
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

						if (b == 0)full.SetTo(temp);
						else full.Add(temp);
					}

					if (transformPositions)
					{
						full.Transform(currentPositionPtr);
						transformedPositionsPtr = transformedPositionsPtrBase + (desc->UniqueVertexIndex * 4);
						BaseVector4_QuickCopy(currentPositionPtr, transformedPositionsPtr);
					}

					if (transformNormals)
					{
						full.Transform(currentVertexNormalPtr);
						full.Transform(currentFaceNormalPtr);
						transformedVertexNormalsPtr = transformedVertexNormalsPtrBase + (desc->UniqueVertexIndex * 4);
						transformedFaceNormalsPtr = transformedFaceNormalsPtrBase + (desc->UniqueVertexIndex * 4);
						BaseVector4_QuickCopy(currentVertexNormalPtr, transformedVertexNormalsPtr);
						BaseVector4_QuickCopy(currentFaceNormalPtr, transformedFaceNormalsPtr);
					}

					if (transformTangents)
					{
						full.Transform(currentVertexTangentPtr);
						transformedVertexTangentsPtr = transformedVertexTangentsPtrBase + (desc->UniqueVertexIndex * 4);
						BaseVector4_QuickCopy(currentVertexTangentPtr, transformedVertexTangentsPtr);
					}

					savedTransforms[desc->UniqueVertexIndex] = full;

					// set position transformation flag to 1 so we don't repeat the work for this vertex if we
					// encounter it again
					cacheFlags[desc->UniqueVertexIndex] = 1;
				}
			}

			// calculate average bone offset
			if (uniqueBonesEncountered == 0)uniqueBonesEncountered = 1;
			averageBoneOffset.MultiplyByScalar(1 / (Real)uniqueBonesEncountered);
			// apply average bone offset to center point
			averageBoneOffset.Transform(centerOut);
		}
	}
}
