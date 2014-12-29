/*********************************************
*
* class: SubMesh3DFaces
*
* author: Mark Kellogg
*
* This class represents an array of sub mesh faces.
*
***********************************************/

#ifndef _SUBMESH3D_FACES_H_
#define _SUBMESH3D_FACES_H_

// forward declarations
class SubMesh3DFace;

#include <vector>
#include <string>
#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

class SubMesh3DFaces
{
	unsigned int faceCount;
	SubMesh3DFace * faces;

	void Destroy();

	public:

	SubMesh3DFaces();
    ~SubMesh3DFaces();

    unsigned int GetFaceCount() const;
    bool Init(unsigned int faceCount);
    SubMesh3DFace * GetFace(unsigned int index);
    const SubMesh3DFace * GetFaceConst(unsigned int index) const;
};

#endif
