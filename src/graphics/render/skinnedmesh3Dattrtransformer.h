#ifndef _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Vector3Array;
class VertexAttrBuffer;
class Skeleton;

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "geometry/point/point3array.h"
#include "graphics/stdattributes.h"

class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
{
	float offset;
	Skeleton * skeleton;
	int vertexBoneMapIndex;
	unsigned char * boneTransformed;

	int vertexTransformedCount;
	unsigned char * vertexTransformed;
	Point3Array transformedVertices;

	void DestroyTransformedBoneFlagsArray();
	bool CreateTransformedBoneFlagsArray();
	void ClearTransformedBoneFlagsArray();

	void DestroyTransformedVertexFlagsArray();
	bool CreateTransformedVertexFlagsArray(unsigned int vertexTransformedCount);
	void ClearTransformedVertexFlagsArray();

	public :

	SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes);
	SkinnedMesh3DAttributeTransformer();
    ~SkinnedMesh3DAttributeTransformer();

    void SetSkeleton(Skeleton * skeleton);
    void SetVertexBoneMapIndex(int index);

    void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut);
    void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut);
};

#endif

