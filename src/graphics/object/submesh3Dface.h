/*********************************************
*
* class: SubMesh3DFace
*
* author: Mark Kellogg
*
* This class represents a single face (polygon) of
* a 3D mesh.
*
***********************************************/

#ifndef _GTE_SUBMESH3D_FACE_H_
#define _GTE_SUBMESH3D_FACE_H_

#include "engine.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

namespace GTE {
    class SubMesh3DFace {
    public:

        SubMesh3DFace();
        ~SubMesh3DFace();

        // index in the respective mesh's attribute arrays of the first
        // vertex of this face
        Int32 FirstVertexIndex;
        // index in the respective mesh's attribute arrays of the first
        // vertex of this face's first adjacent face
        Int32 AdjacentFaceIndex1;
        // index in the respective mesh's attribute arrays of the second
        // vertex of this face's first adjacent face
        Int32 AdjacentFaceIndex2;
        // index in the respective mesh's attribute arrays of the third
        // vertex of this face's first adjacent face
        Int32 AdjacentFaceIndex3;
    };
}

#endif

