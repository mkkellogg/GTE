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
	AttributeTransformer();
    virtual ~AttributeTransformer();

    virtual void TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut, const Vector3Array& normalsIn, Vector3Array& normalsOut) = 0;
    virtual void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut) = 0;
    virtual void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut) = 0;

    void SetActiveAttributes (StandardAttributeSet attributes);
    StandardAttributeSet GetActiveAttributes();
};

#endif

