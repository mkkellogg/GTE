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

#include "engine.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

#include <vector>
#include <string>

namespace GTE
{
	// forward declarations
	class SubMesh3DFace;

	class SubMesh3DFaces
	{
		// number of faces in [faces]
		UInt32 faceCount;
		// face data array
		SubMesh3DFace * faces;

		void Destroy();

	public:

		SubMesh3DFaces();
		~SubMesh3DFaces();

		UInt32 GetFaceCount() const;
		Bool Init(UInt32 faceCount);
		SubMesh3DFace * GetFace(UInt32 index);
		const SubMesh3DFace * GetFaceConst(UInt32 index) const;
	};
}

#endif
