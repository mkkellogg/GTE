#ifndef _ATTRIBUTE_TRANSFORMER_H
#define _ATTRIBUTE_TRANSFORMER_H

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;
class Point3;

#include "graphics/stdattributes.h"
#include "geometry/matrix4x4.h"

class AttributeTransformer
{
	protected:

	StandardAttributeSet activeAttributes;
	Matrix4x4 model;
	Matrix4x4 modelInverse;

	public :

	AttributeTransformer(StandardAttributeSet attributes);
	AttributeTransformer();
    virtual ~AttributeTransformer();

    virtual void TransformPositionsAndNormals(const Point3Array& positionsIn,  Point3Array& positionsOut, const Vector3Array& normalsIn, Vector3Array& normalsOut, const Point3& centerIn, Point3& centerOut) = 0;
    virtual void TransformPositions(const Point3Array& positionsIn,  Point3Array& positionsOut, const Point3& centerIn, Point3& centerOut) = 0;
    virtual void TransformNormals(const Vector3Array& normalsIn, Vector3Array& normalsOut) = 0;

    void SetActiveAttributes (StandardAttributeSet attributes);
    StandardAttributeSet GetActiveAttributes() const;

    void SetModelMatrix(const Matrix4x4& model, const Matrix4x4& modelInverse);
};

#endif

