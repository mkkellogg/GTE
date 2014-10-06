#ifndef _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;

#include "attributetransformer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include "graphics/stdattributes.h"

class SkinnedMesh3DAttributeTransformer : public AttributeTransformer
{
	float offset;
	public :

	SkinnedMesh3DAttributeTransformer(StandardAttributeSet attributes);
	SkinnedMesh3DAttributeTransformer();
    ~SkinnedMesh3DAttributeTransformer();

    void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut);
    void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut);
};

#endif

