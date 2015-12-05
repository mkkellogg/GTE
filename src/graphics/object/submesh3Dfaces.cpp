#include "submesh3Dface.h"
#include "submesh3Dfaces.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	* Base constructor - initializes all member variables.
	*/
	SubMesh3DFaces::SubMesh3DFaces()
	{
		faceCount = 0;
		faces = nullptr;
	}

	/*
	 * Clean up.
	 */
	SubMesh3DFaces::~SubMesh3DFaces()
	{
		Destroy();
	}

	/*
	 * Delete all instances of SubMesh3DFace in [faces] as well
	 * as the array itself.
	 */
	void SubMesh3DFaces::Destroy()
	{
		SAFE_DELETE_ARRAY(faces);
	}

	/*
	 * Get the number of faces in [faces].
	 */
	UInt32 SubMesh3DFaces::GetFaceCount() const
	{
		return faceCount;
	}

	/*
	 * Initialization, allocates [faces].
	 */
	Bool SubMesh3DFaces::Init(UInt32 faceCount)
	{
		Destroy();

		this->faceCount = faceCount;
		faces = new(std::nothrow) SubMesh3DFace[faceCount];

		ASSERT(faces != nullptr, "SubMesh3DFaces::Init -> Could not allocate faces array.");

		return true;
	}

	/*
	 * Get a pointer to the face at [index] in [faces].
	 */
	SubMesh3DFace * SubMesh3DFaces::GetFace(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < faceCount, "SubMesh3DFaces::GetFace -> 'index' is out of range.", nullptr, true);
		return faces + index;
	}


	/*
	 * Get a const pointer to the face at [index] in [faces].
	 */
	const SubMesh3DFace * SubMesh3DFaces::GetFaceConst(UInt32 index) const
	{
		NONFATAL_ASSERT_RTRN(index < faceCount, "SubMesh3DFaces::GetFaceConst -> 'index' is out of range.", nullptr, true);
		return (const SubMesh3DFace *)(faces + index);
	}
}
