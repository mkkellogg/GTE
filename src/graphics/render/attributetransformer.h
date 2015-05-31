/*
 * class:  AttributeTransformer
 *
 * Author: Mark Kellogg
 *
 * Base class for performing modifications to mesh attribute structures.
 */


#ifndef _GTE_ATTRIBUTE_TRANSFORMER_H
#define _GTE_ATTRIBUTE_TRANSFORMER_H

#include "graphics/stdattributes.h"
#include "object/enginetypes.h"
#include "geometry/matrix4x4.h"

namespace GTE
{
	//forward declarations
	class Point3Array;
	class Vector3Array;
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
			bool transformPositions, bool transformNormals, bool transformTangents) = 0;

		void SetActiveAttributes(StandardAttributeSet attributes);
		StandardAttributeSet GetActiveAttributes() const;

		void SetModelMatrix(const Matrix4x4& model, const Matrix4x4& modelInverse);
	};
}

#endif

