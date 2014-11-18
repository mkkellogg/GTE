#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "engine.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/render/material.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "gameutil.h"

Mesh3DRef GameUtil::CreateCubeMesh(StandardAttributeSet meshAttributes)
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	SubMesh3DRef subMesh = objectManager->CreateSubMesh3D(meshAttributes);
	subMesh->Init(36);

	Mesh3DRef mesh = objectManager->CreateMesh3D(1);
	mesh->Init();
	mesh->SetSubMesh(subMesh, 0);

	Point3Array * points = subMesh->GetPostions();
	Color4Array * colors = subMesh->GetColors();
	UV2Array *uvs = NULL;

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))
	{
		uvs = subMesh->GetUVsTexture0();
	}

	// --- Cube vertices -------
	// cube front, triangle 1
	points->GetPoint(0)->Set(-1, 1, 1);
	points->GetPoint(1)->Set(1, 1, 1);
	points->GetPoint(2)->Set(-1, -1, 1);

	// cube front, triangle 2
	points->GetPoint(3)->Set(1, 1, 1);
	points->GetPoint(4)->Set(1, -1, 1);
	points->GetPoint(5)->Set(-1, -1, 1);

	// cube right, triangle 1
	points->GetPoint(6)->Set(1, 1, 1);
	points->GetPoint(7)->Set(1, 1, -1);
	points->GetPoint(8)->Set(1, -1, 1);

	// cube right, triangle 2
	points->GetPoint(9)->Set(1, 1, -1);
	points->GetPoint(10)->Set(1, -1, -1);
	points->GetPoint(11)->Set(1, -1, 1);

	// cube left, triangle 1
	points->GetPoint(12)->Set(-1, 1, -1);
	points->GetPoint(13)->Set(-1, 1, 1);
	points->GetPoint(14)->Set(-1, -1, -1);

	// cube left, triangle 2
	points->GetPoint(15)->Set(-1, 1, 1);
	points->GetPoint(16)->Set(-1, -1, 1);
	points->GetPoint(17)->Set(-1, -1, -1);

	// cube top, triangle 1
	points->GetPoint(18)->Set(-1, 1, -1);
	points->GetPoint(19)->Set(1, 1, -1);
	points->GetPoint(20)->Set(-1, 1, 1);

	// cube top, triangle 2
	points->GetPoint(21)->Set(1, 1, -1);
	points->GetPoint(22)->Set(1, 1, 1);
	points->GetPoint(23)->Set(-1, 1, 1);

	// cube back, triangle 1
	points->GetPoint(24)->Set(1, 1, -1);
	points->GetPoint(25)->Set(-1, 1, -1);
	points->GetPoint(26)->Set(1, -1, -1);

	// cube back, triangle 2
	points->GetPoint(27)->Set(-1, 1, -1);
	points->GetPoint(28)->Set(-1, -1, -1);
	points->GetPoint(29)->Set(1, -1, -1);

	// cube bottom, triangle 1
	points->GetPoint(30)->Set(-1, -1, -1);
	points->GetPoint(31)->Set(-1, -1, 1);
	points->GetPoint(32)->Set(1, -1, 1);

	// cube bottom, triangle 2
	points->GetPoint(33)->Set(-1, -1, -1);
	points->GetPoint(34)->Set(1, -1, 1);
	points->GetPoint(35)->Set(1, -1, -1);

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))
	{
		// --- Cube colors -------
			// cube front, triangle 1

		for(int i=0; i< 36; i++)
		{
			colors->GetColor(i)->Set(1,1,1,1);
		}
	}

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))
	{
		// --- Cube UVs -------
		// cube front, triangle 1
		uvs->GetCoordinate(0)->Set(0,0);
		uvs->GetCoordinate(1)->Set(1,0);
		uvs->GetCoordinate(2)->Set(0,1);

		// cube front, triangle 2
		uvs->GetCoordinate(3)->Set(1,0);
		uvs->GetCoordinate(4)->Set(1,1);
		uvs->GetCoordinate(5)->Set(0,1);

		// cube right, triangle 1
		uvs->GetCoordinate(6)->Set(0,0);
		uvs->GetCoordinate(7)->Set(1,0);
		uvs->GetCoordinate(8)->Set(0,1);

		// cube right, triangle 2
		uvs->GetCoordinate(9)->Set(1,0);
		uvs->GetCoordinate(10)->Set(1,1);
		uvs->GetCoordinate(11)->Set(0,1);

		// cube left, triangle 1
		uvs->GetCoordinate(12)->Set(0,0);
		uvs->GetCoordinate(13)->Set(1,0);
		uvs->GetCoordinate(14)->Set(0,1);

		// cube left, triangle 2
		uvs->GetCoordinate(15)->Set(1,0);
		uvs->GetCoordinate(16)->Set(1,1);
		uvs->GetCoordinate(17)->Set(0,1);

		// cube top, triangle 1
		uvs->GetCoordinate(18)->Set(0,0);
		uvs->GetCoordinate(19)->Set(1,0);
		uvs->GetCoordinate(20)->Set(0,1);

		// cube top, triangle 2
		uvs->GetCoordinate(21)->Set(1,0);
		uvs->GetCoordinate(22)->Set(1,1);
		uvs->GetCoordinate(23)->Set(0,1);

		// cube back, triangle 1
		uvs->GetCoordinate(24)->Set(0,0);
		uvs->GetCoordinate(25)->Set(1,0);
		uvs->GetCoordinate(26)->Set(0,1);

		// cube back, triangle 2
		uvs->GetCoordinate(27)->Set(1,0);
		uvs->GetCoordinate(28)->Set(1,1);
		uvs-> GetCoordinate(29)->Set(0,1);

		// cube back, triangle 1
		uvs->GetCoordinate(30)->Set(0,0);
		uvs->GetCoordinate(31)->Set(1,0);
		uvs->GetCoordinate(32)->Set(0,1);

		// cube back, triangle 2
		uvs->GetCoordinate(33)->Set(1,0);
		uvs->GetCoordinate(34)->Set(1,1);
		uvs-> GetCoordinate(35)->Set(0,1);
	}

	subMesh->SetNormalsSmoothingThreshold(85);
	subMesh->Update();
	return mesh;
}
