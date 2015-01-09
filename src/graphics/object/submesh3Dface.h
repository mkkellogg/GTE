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

#include <vector>
#include <string>
#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

class SubMesh3DFace
{
	public:

	SubMesh3DFace();
	~SubMesh3DFace();

	int FirstVertexIndex;
	int AdjacentFaceIndex1;
	int AdjacentFaceIndex2;
	int AdjacentFaceIndex3;
};

#endif

