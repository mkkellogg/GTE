#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include "submesh3Dface.h"
#include "submesh3Dfaces.h"
#include "global/global.h"
#include "debug/gtedebug.h"

SubMesh3DFaces::SubMesh3DFaces()
{
	faceCount = 0;
	faces = NULL;
}

SubMesh3DFaces::~SubMesh3DFaces()
{
	Destroy();
}

void SubMesh3DFaces::Destroy()
{
	if(faces != NULL)
	{
		delete[] faces;
		faces = NULL;
	}
}

unsigned int SubMesh3DFaces::GetFaceCount() const
{
	return faceCount;
}

bool SubMesh3DFaces::Init(unsigned int faceCount)
{
	Destroy();

	this->faceCount = faceCount;
	faces = new SubMesh3DFace[faceCount];

	ASSERT(faces != NULL, "SubMesh3DFaces::Init -> Could not allocate faces array.", false);

	return true;
}

SubMesh3DFace * SubMesh3DFaces::GetFace(unsigned int index)
{
	ASSERT(index < faceCount, "SubMesh3DFaces::GetFace -> Index is out of range.", NULL);
	return faces + index;
}

const SubMesh3DFace * SubMesh3DFaces::GetFaceConst(unsigned int index) const
{
	ASSERT(index < faceCount, "SubMesh3DFaces::GetFaceConst -> Index is out of range.", NULL);
	return (const SubMesh3DFace *)(faces + index);
}
