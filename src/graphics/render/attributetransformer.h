/*
 * class:  AttributeTransformer
 *
 * Author: Mark Kellogg
 *
 * Base class for performing modifications to mesh attribute structures.
 */


#ifndef _GTE_ATTRIBUTE_TRANSFORMER_H
#define _GTE_ATTRIBUTE_TRANSFORMER_H

#include "engine.h"
#include "graphics/stdattributes.h"
#include "geometry/matrix4x4.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3.h"

namespace GTE
{
	//forward declarations
	class VertexAttrBuffer;
	class Point3;

	class AttributeTransformer
	{
	protected:

		StandardAttributeSet activeAttributes;
		Matrix4x4 model;
		Matrix4x4 modelInverse;

	public:

		AttributeTransformer(StandardAttributeSet attributes);
		AttributeTransformer();
		virtual ~AttributeTransformer();

		virtual void TransformAttributes(const Point3Array& positionsIn, Point3Array& positionsOut,
			const Vector3Array& vertexNormalsIn, Vector3Array& vertexNormalsOut,
			const Vector3Array& faceNormalsIn, Vector3Array& faceNormalsOut,
			const Vector3Array& vertexTangentsIn, Vector3Array& vertexTangentsOut,
			const Point3& centerIn, Point3& centerOut,
			Bool transformPositions, Bool transformNormals, Bool transformTangents) = 0;

		void SetActiveAttributes(StandardAttributeSet attributes);
		StandardAttributeSet GetActiveAttributes() const;

		void SetModelMatrix(const Matrix4x4& model, const Matrix4x4& modelInverse);
	};
}

#endif

