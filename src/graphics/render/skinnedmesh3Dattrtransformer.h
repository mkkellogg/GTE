/*
 * class:  SkinnedMesh3DAttributeTransformer
 *
 * Author: Mark Kellogg
 *
 * This class does the work of CPU-side vertex skinning. It contains
 * functions that take the normal and position arrays from a mesh as
 * input and transform them according a bone structure specified by
 * [skeleton].
 */

#ifndef _GTE_SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _GTE_SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

#include "engine.h"
#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/stdattributes.h"

namespace GTE
{
	//forward declarations
	class Vector3Array;
	class VertexAttrBuffer;
	class Skeleton;
	class Matrix4x4;
	class VertexBoneMap;

	class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
	{
		enum class CacheType
		{
			Position = 0,
			VertexNormal = 1,
			FaceNormal = 2,
			VertexTangent = 3,
			Transform = 4
		};

		// the renderer for which this transformer acts
		SkinnedMesh3DRenderer* renderer;
		// [renderer] has an array of VertexBoneMap objects. [vertexBoneMapIndex] is the
		// index in that array that contains the VertexBoneMap for this instance of
		// SkinnedMesh3DAttributeTransformer.
		Int32 vertexBoneMapIndex;

		Int32 boneCount;
		// flag for each Bone object in the list of bones held by the skeleton in [renderer]. the flag indicates
		// whether the transformation for that bone has been calculated already (bones are often visited
		// multiple times during a single vertex skinning operation).
		UChar * boneTransformed;


		// existing size of each cache
		Int32 currentCacheSize;
		// flags that indicate that an entry in any cache at a specified index is valid
		UChar * cacheFlags;

		// once the full transformation has been calculated for a vertex, save it for later reuse
		Matrix4x4 * savedTransforms;

		// saved values of vertices that have been transformed
		Point3Array transformedPositions;

		// saved values of vertex normals that have been transformed
		Vector3Array transformedVertexNormals;

		// saved values of face normals that have been transformed
		Vector3Array transformedFaceNormals;

		// saved values of vertex tangents that have been transformed
		Vector3Array transformedVertexTangents;

		// flag for each (unique) vertex that indicates whether the normals for all instances of that
		// vertex are equal
		UChar * identicalNormalFlags;

		// flag for each (unique) vertex that indicates whether the tangents for all instances of that
		// vertex are equal
		UChar * identicalTangentFlags;

		void UpdateTransformedBoneCacheSize();
		void DestroyTransformedBoneFlagsArray();
		Bool CreateTransformedBoneFlagsArray();
		void ClearTransformedBoneFlagsArray();

		void DestroyCache(CacheType target);
		Bool CreateCache(CacheType target);
		void ClearCacheFlags();
		void SetAllTransformCacheFlags(UChar value);

		void DestroyIdenticalNormalsTangentsFlags();
		Bool CreateIdenticalNormalsTangentsFlags();
		void ClearIdenticalNormalsTangentsFlags();
		Bool FindIdenticalNormalsOrTangents(Vector3Array& fullNormalLists, Bool forNormals);

		Bool CreateCaches();
		void DestroyCaches();

	public:

		SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes);
		SkinnedMesh3DAttributeTransformer();
		~SkinnedMesh3DAttributeTransformer();

		void SetRenderer(SkinnedMesh3DRenderer* renderer);
		void SetVertexBoneMapIndex(Int32 index);

		void TransformAttributes(const Point3Array& positionsIn, Point3Array& positionsOut,
			const Vector3Array& vertexNormalsIn, Vector3Array& vertexNormalsOut,
			const Vector3Array& faceNormalsIn, Vector3Array& faceNormalsOut,
			const Vector3Array& vertexTangentsIn, Vector3Array& vertexTangentsOut,
			const Point3& centerIn, Point3& centerOut,
			Bool transformPositions, Bool transformNormals, Bool transformTangents);
	};
}

#endif

