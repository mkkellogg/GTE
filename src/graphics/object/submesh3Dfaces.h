/*********************************************
*
* class: SubMesh3DFaces
*
* author: Mark Kellogg
*
* This class contains an array of sub mesh faces, specifically
* instances of SubMesh3DFace.
*
***********************************************/

#ifndef _GTE_SUBMESH3D_FACES_H_
#define _GTE_SUBMESH3D_FACES_H_

#include <vector>
#include <string>
#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

namespace GTE
{
	// forward declarations
	class SubMesh3DFace;

	class SubMesh3DFaces
	{
		// number of faces in [faces]
		unsigned int faceCount;
		// face data array
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
}

#endif
