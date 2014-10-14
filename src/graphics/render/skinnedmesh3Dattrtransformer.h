#ifndef _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;
class Skeleton;

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "graphics/stdattributes.h"

class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
{
	float offset;
	Skeleton * skeleton;
	int vertexBoneMapIndex;

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

