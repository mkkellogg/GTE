/*
 * class: Util
 *
 * author: Mark Kellogg
 *
 * General utility class for the engine.
 *
 */

#ifndef _GTE_ENGINEUTIL_H_
#define _GTE_ENGINEUTIL_H_

#include "engine.h"
#include "graphics/stdattributes.h"

#include <string>

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
		static Mesh3DSharedPtr CreateRectangularMesh(StandardAttributeSet meshAttributes, Real width, Real height,
			UInt32 subDivisionsWidth, UInt32 subDivisionsHeight,
			Bool calculateNormals, Bool calculateTangents, Bool buildFaces);
		static Mesh3DSharedPtr CreateCubeMesh(StandardAttributeSet meshAttributes, Bool doCCW);
		static Mesh3DSharedPtr CreateCubeMesh(StandardAttributeSet meshAttributes);
		/*template <class B, class D> static Bool EngineUtility::InstanceOf(B& base)
		{
			try
			{
				D& d = dynamic_cast<D&>(base);
				return true;
			}
			catch(std::bad_cast e)
			{
				return false;
			}
		}*/
	};
}

#endif
