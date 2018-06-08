#include "importutil.h"
#include "geometry/matrix4x4.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE {
    void ImportUtil::ConvertAssimpMatrix(const aiMatrix4x4& source, Matrix4x4& dest) {
        Real data[16];

        data[0] = source.a1;
        data[1] = source.b1;
        data[2] = source.c1;
        data[3] = source.d1;

        data[4] = source.a2;
        data[5] = source.b2;
        data[6] = source.c2;
        data[7] = source.d2;

        data[8] = source.a3;
        data[9] = source.b3;
        data[10] = source.c3;
        data[11] = source.d3;

        data[12] = source.a4;
        data[13] = source.b4;
        data[14] = source.c4;
        data[15] = source.d4;

        dest.SetTo(data);
    }
}

