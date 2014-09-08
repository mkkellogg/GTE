#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <string>

#include "importutil.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "geometry/matrix4x4.h"
#include "ui/debug.h"
#include "global/global.h"

void ImportUtil::ConvertAssimpMatrix(const aiMatrix4x4 * source, Matrix4x4 * dest)
{
	NULL_CHECK_RTRN(source,"ImportUtil::ConvertAssimpMatrix -> source is NULL.");
	NULL_CHECK_RTRN(dest,"ImportUtil::ConvertAssimpMatrix -> dest is NULL.");

	float data[16];

	data[0] = source->a1;
	data[1] = source->b1;
	data[2] = source->c1;
	data[3] = source->d1;

	data[4] = source->a2;
	data[5] = source->b2;
	data[6] = source->c2;
	data[7] = source->d2;

	data[8] = source->a3;
	data[9] = source->b3;
	data[10] = source->c3;
	data[11] = source->d3;

	data[12] = source->a4;
	data[13] = source->b4;
	data[14] = source->c4;
	data[15] = source->d4;

	dest->SetTo(data);
}
