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

//forward declarations
class Matrix4x4;

#include <string>
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"

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
	static Mesh3DRef CreateCubeMesh(StandardAttributeSet meshAttributes);
};

#endif