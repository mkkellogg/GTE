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

class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
{
	float offset;
	Skeleton * skeleton;
	int vertexBoneMapIndex;
	unsigned char * boneTransformed;

	Matrix4x4 * savedTransforms;

	int positionTransformedCount;
	unsigned char * positionTransformed;
	Point3Array transformedPositions;

	int normalTransformedCount;
	unsigned char * normalTransformed;
	Vector3Array transformedNormals;

	void DestroySavedTransformsArray();
	bool CreateSavedTransformsArray(unsigned int saveCount);

	void DestroyTransformedBoneFlagsArray();
	bool CreateTransformedBoneFlagsArray();
	void ClearTransformedBoneFlagsArray();

	void DestroyTransformedPositionFlagsArray();
	bool CreateTransformedPositionFlagsArray(unsigned int positionTransformedCount);
	void ClearTransformedPositionFlagsArray();

	void DestroyTransformedNormalFlagsArray();
	bool CreateTransformedNormalFlagsArray(unsigned int normalTransformedCount);
	void ClearTransformedNormalFlagsArray();

	public :

	SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes);
	SkinnedMesh3DAttributeTransformer();
    ~SkinnedMesh3DAttributeTransformer();

    void SetSkeleton(Skeleton * skeleton);
    void SetVertexBoneMapIndex(int index);

    void TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut, const Vector3Array& normalsIn, Vector3Array& normalsOut);
    void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut);
    void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut);
};

#endif

