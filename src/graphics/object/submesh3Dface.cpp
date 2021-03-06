#include "submesh3Dface.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE {
    /*
    * Constructor - initialize all member variables.
    */
    SubMesh3DFace::SubMesh3DFace() {
        FirstVertexIndex = -1;
        AdjacentFaceIndex1 = -1;
        AdjacentFaceIndex2 = -1;
        AdjacentFaceIndex3 = -1;
    }

    /*
     * Clean up.
     */
    SubMesh3DFace::~SubMesh3DFace() {

    }
}
