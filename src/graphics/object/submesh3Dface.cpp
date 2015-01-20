#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include "submesh3Dface.h"
#include "global/global.h"
#include "debug/gtedebug.h"

SubMesh3DFace::SubMesh3DFace()
{
	FirstVertexIndex = -1;
	AdjacentFaceIndex1 = -1;
	AdjacentFaceIndex2 = -1;
	AdjacentFaceIndex3 = -1;
}

SubMesh3DFace::~SubMesh3DFace()
{

}
