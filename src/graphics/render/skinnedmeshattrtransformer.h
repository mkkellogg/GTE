#ifndef _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H
#define _SKINNEDMESH_ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;

#include "attributetransformer.h"
#include "skinnedmeshattrtransformer.h"
#include "graphics/stdattributes.h"

class SkinnedMeshAttributeTransformer : public AttributeTransformer
{
	float offset;
	public :

	SkinnedMeshAttributeTransformer(StandardAttributeSet attributes);
	SkinnedMeshAttributeTransformer();
    ~SkinnedMeshAttributeTransformer();

    void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut);
    void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut);
};

#endif

