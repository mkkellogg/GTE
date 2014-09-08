#ifndef _IMPORT_UTIL_H
#define _IMPORT_UTIL_H

//forward declarations

#include "assimp/scene.h"
#include "geometry/matrix4x4.h"
#include <string>

class ImportUtil
{
	public:

	static void ConvertAssimpMatrix(const aiMatrix4x4 * source, Matrix4x4 * dest);
};

#endif
