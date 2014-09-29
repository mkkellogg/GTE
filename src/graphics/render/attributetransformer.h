#ifndef _ATTRIBUTE_TRANSFORMER_H
#define _ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;

#include "graphics/stdattributes.h"

class AttributeTransformer
{
	StandardAttributeSet activeAttributes;

	public :

	AttributeTransformer(StandardAttributeSet attributes);
    virtual ~AttributeTransformer();

    virtual void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut) = 0;
    virtual void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut) = 0;

    StandardAttributeSet GetActiveAttributes();
};

#endif

