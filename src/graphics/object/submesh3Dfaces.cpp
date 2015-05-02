#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include "submesh3Dface.h"
#include "submesh3Dfaces.h"
#include "global/global.h"
#include "debug/gtedebug.h"

/*
 * Base constructor - initializes all member variables.
 */
SubMesh3DFaces::SubMesh3DFaces()
{
	faceCount = 0;
	faces = NULL;
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
	if(faces != NULL)
	{
		delete[] faces;
		faces = NULL;
	}
}

/*
 * Get the number of faces in [faces].
 */
unsigned int SubMesh3DFaces::GetFaceCount() const
{
	return faceCount;
}

/*
 * Initialization, allocates [faces].
 */
bool SubMesh3DFaces::Init(unsigned int faceCount)
{
	Destroy();

	this->faceCount = faceCount;
	faces = new SubMesh3DFace[faceCount];

	ASSERT(faces != NULL, "SubMesh3DFaces::Init -> Could not allocate faces array.");

	return true;
}

/*
 * Get a pointer to the face at [index] in [faces].
 */
SubMesh3DFace * SubMesh3DFaces::GetFace(unsigned int index)
{
	NONFATAL_ASSERT_RTRN(index < faceCount, "SubMesh3DFaces::GetFace -> 'index' is out of range.", NULL, true);
	return faces + index;
}


/*
 * Get a const pointer to the face at [index] in [faces].
 */
const SubMesh3DFace * SubMesh3DFaces::GetFaceConst(unsigned int index) const
{
	NONFATAL_ASSERT_RTRN(index < faceCount, "SubMesh3DFaces::GetFaceConst -> 'index' is out of range.", NULL, true);
	return (const SubMesh3DFace *)(faces + index);
}
