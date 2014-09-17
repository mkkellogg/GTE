#ifndef _IMPORT_UTIL_H
#define _IMPORT_UTIL_H

//forward declarations
class RawImage;

#include "assimp/scene.h"
#include "geometry/matrix4x4.h"
#include <string>
#include <IL/il.h>

class ImportUtil
{
	public:

	static void ConvertAssimpMatrix(const aiMatrix4x4 * source, Matrix4x4 * dest);
	static RawImage * GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height);
};

#endif
