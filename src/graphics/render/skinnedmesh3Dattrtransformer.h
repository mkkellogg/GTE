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

#ifndef _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Vector3Array;
class VertexAttrBuffer;
class Skeleton;
class Matrix4x4;

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/stdattributes.h"
#include "object/enginetypes.h"

class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
{
	enum class Target
	{
		Position = 0,
		Normal = 1,
		StraightNormal = 2,
		IdenticalNormal = 3,
		Transform = 4
	};

	// the Skeleton object that contains the bone structure that determines how
	// the mesh attributes will be transformed.
	SkeletonRef skeleton;
	// [skeleton] has an array of VertexBoneMap objects. [vertexBoneMapIndex] is the
	// index in that array that contains the VertexBoneMap for this instance of
	// SkinnedMesh3DAttributeTransformer.
	int vertexBoneMapIndex;
	// flag for each Bone object in the list of bones held by [skeleton]. the flag indicates
	// whether the transformation for that bone has been calculated already (bones are often visited
	// multiple times during a single vertex skinning operation).
	unsigned char * boneTransformed;

	// size of the [savedTransforms] array
	int savedTransformCount;
	// flag for each (unique) vertex that is transformed indicating that a transform
	// has been calculated for it
	unsigned char * transformCalculated;
	// once the full transformation has been calculated for a vertex, save it for later reuse
	Matrix4x4 * savedTransforms;

	// size of the [positionTransformed] array
	int positionTransformedCount;
	// flag for each (unique) vertex that is transformed indicating that it has been visited and transformed.
	// used to avoid transforming a single vertex multiple times
	unsigned char * positionTransformed;
	// saved values of vertices that have been transformed
	Point3Array transformedPositions;

	// size of the [normalTransformed] array
	int normalTransformedCount;
	// flag for each (unique) normal that is transformed indicating that it has been visited and transformed.
	// used to avoid transforming a single normal multiple times
	unsigned char * normalTransformed;
	// saved values of normals that have been transformed
	Vector3Array transformedNormals;

	// size of the [straightNormalTransformed] array
	int straightNormalTransformedCount;
	// flag for each (unique) straight normal that is transformed indicating that it has been visited and transformed.
	// used to avoid transforming a single straight normal multiple times
	unsigned char * straightNormalTransformed;
	// saved values of straight normals that have been transformed
	Vector3Array transformedStraightNormals;

	// size of [identicalNormals] array
	int identicalNormalCount;
	// flag for each (unique) vertex that indicates whether the normals for all instances of that
	// vertex are equal
	unsigned char * identicalNormals;

	void DestroySavedTransformsArray();
	bool CreateSavedTransformsArray(unsigned int saveCount);

	void DestroyTransformedBoneFlagsArray();
	bool CreateTransformedBoneFlagsArray();
	void ClearTransformedBoneFlagsArray();

	void DestroyTransformCache(Target target);
	bool CreateTransformCache(Target target, unsigned int count);
	void ClearTransformCache(Target target);
	void SetAllTransformCache(Target target, unsigned char value);

	void FindIdenticalNormals(Vector3Array& fullNormalLists);

	public :

	SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes);
	SkinnedMesh3DAttributeTransformer();
    ~SkinnedMesh3DAttributeTransformer();

    void SetSkeleton(SkeletonRef skeleton);
    void SetVertexBoneMapIndex(int index);

    void TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut,
    		 	 	 	 	 	 	  const Vector3Array& normalsIn, Vector3Array& normalsOut,
    		 	 	 	 	 	 	  const Vector3Array& straightNormalsIn, Vector3Array& straightNormalsOut,
    		 	 	 	 	 	 	  const Point3& centerIn, Point3& centerOut);
    void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut, const Point3& centerIn, Point3& centerOut);
    void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut,
    					  const Vector3Array& straightNormalsIn, Vector3Array& straightNormalsOut);
};

#endif

