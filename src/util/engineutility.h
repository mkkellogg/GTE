/*
 * class: Util
 *
 * author: Mark Kellogg
 *
 * General utility class
 *
 */

#ifndef _GTE_ENGINEUTIL_H_
#define _GTE_ENGINEUTIL_H_

#include <string>
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"

namespace GTE
{
	//forward declarations
	class Matrix4x4;

	enum EngineUtilityError
	{
		InvalidDimensions = 1
	};

	class EngineUtility
	{
	protected:

		EngineUtility();
		~EngineUtility();

	public:

		static std::string TrimLeft(const std::string& str);
		static std::string TrimRight(const std::string& str);
		static std::string Trim(const std::string& str);
		static void PrintMatrix(const Matrix4x4& matrix);
		static Mesh3DRef CreateRectangularMesh(StandardAttributeSet meshAttributes, Real width, Real height,
			UInt32 subDivisionsWidth, UInt32 subDivisionsHeight,
			Bool calculateNormals, Bool calculateTangents, Bool buildFaces);
		static Mesh3DRef CreateCubeMesh(StandardAttributeSet meshAttributes, Bool doCCW);
		static Mesh3DRef CreateCubeMesh(StandardAttributeSet meshAttributes);
	};
}

#endif
