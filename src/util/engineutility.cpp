#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "engineutility.h"
#include "geometry/matrix4x4.h"
#include "engine.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendertarget.h"
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

namespace GTE
{
	EngineUtility::EngineUtility()
	{

	}

	EngineUtility::~EngineUtility()
	{

	}

	std::string EngineUtility::TrimLeft(const std::string& str)
	{
		std::size_t first = str.find_first_not_of(' ');
		return str.substr(first, str.size());
	}

	std::string EngineUtility::TrimRight(const std::string& str)
	{
		std::size_t last = str.find_last_not_of(' ');
		return str.substr(0, last + 1);
	}

	std::string EngineUtility::Trim(const std::string& str)
	{
		std::size_t first = str.find_first_not_of(' ');
		std::size_t last = str.find_last_not_of(' ');
		return str.substr(first, last - first + 1);
	}

	void EngineUtility::PrintMatrix(const Matrix4x4& matrix)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			for (UInt32 j = 0; j < 4; j++)
			{
				if (j>0)printf(",");
				printf("%f", matrix.GetDataPtr()[i * 4 + j]);
			}
			printf("\n");
		}
	}

	Mesh3DRef EngineUtility::CreateRectangularMesh(StandardAttributeSet meshAttributes, Real width, Real height,
		UInt32 subDivisionsPerWidth, UInt32 subDivisionsPerHeight,
		bool calculateNormals, bool calculateTangents, bool buildFaces)
	{
		if (width <= 0 || height <= 0)
		{
			Engine::Instance()->GetErrorManager()->SetError(EngineUtilityError::InvalidDimensions, "EngineUtility::CreateSquareMesh -> Width and height must be greater than 0.");
			return Mesh3DRef::Null();
		}

		// force mesh to have at least positions and normals
		if (calculateNormals)
		{
			StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
			StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
		}
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

		UInt32 subSquaresPerWidth = subDivisionsPerWidth;
		UInt32 subSquaresPerHeight = subDivisionsPerHeight;
		UInt32 totalSubSquares = subSquaresPerWidth * subSquaresPerHeight;
		UInt32 totalVertices = totalSubSquares * 6;

		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
		SubMesh3DRef subMesh = objectManager->CreateSubMesh3D(meshAttributes);
		subMesh->Init(totalVertices);

		Mesh3DRef mesh = objectManager->CreateMesh3D(1);
		mesh->Init();

		Point3Array * points = subMesh->GetPostions();
		UV2Array *uvs = NULL;

		Real halfWidth = width / 2.0;
		Real halfHeight = height / 2.0;

		Real initialWidth = -halfWidth;
		Real initialHeight = halfHeight;

		Real currentWidth = initialWidth;
		Real currentHeight = initialHeight;

		Real subSquareWidth = width / (Real)subSquaresPerWidth;
		Real subSquareHeight = height / (Real)subSquaresPerWidth;

		Real uvWidth = 1.0;
		Real uvHeight = 1.0;

		Real subSquareUVWidth = uvWidth / (Real)subSquaresPerWidth;
		Real subSquareUVHeight = uvHeight / (Real)subSquaresPerHeight;

		UInt32 vertexIndex = 0;

		for (UInt32 heightSquare = 0; heightSquare < subSquaresPerHeight; heightSquare++)
		{
			for (UInt32 widthSquare = 0; widthSquare < subSquaresPerWidth; widthSquare++)
			{
				points->GetPoint(vertexIndex)->Set(currentWidth, currentHeight, 0);
				points->GetPoint(vertexIndex + 1)->Set(currentWidth + subSquareWidth, currentHeight, 0);
				points->GetPoint(vertexIndex + 2)->Set(currentWidth, currentHeight - subSquareHeight, 0);

				points->GetPoint(vertexIndex + 3)->Set(currentWidth + subSquareWidth, currentHeight, 0);
				points->GetPoint(vertexIndex + 4)->Set(currentWidth + subSquareWidth, currentHeight - subSquareHeight, 0);
				points->GetPoint(vertexIndex + 5)->Set(currentWidth, currentHeight - subSquareHeight, 0);

				Real uvX = (currentWidth + halfWidth) / width;
				Real uvY = ((currentHeight + halfHeight) / height);

				if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0) || StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))
				{
					for (UInt32 i = 0; i < 2; i++)
					{
						if (i == 0)
						{
							if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))
								uvs = subMesh->GetUVs0();
							else continue;
						}

						if (i == 1)
						{
							if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))
								uvs = subMesh->GetUVs1();
							else continue;
						}

						uvs->GetCoordinate(vertexIndex)->Set(uvX, uvY);
						uvs->GetCoordinate(vertexIndex + 1)->Set(uvX + subSquareUVWidth, uvY);
						uvs->GetCoordinate(vertexIndex + 2)->Set(uvX, uvY - subSquareUVHeight);

						uvs->GetCoordinate(vertexIndex + 3)->Set(uvX + subSquareUVWidth, uvY);
						uvs->GetCoordinate(vertexIndex + 4)->Set(uvX + subSquareUVWidth, uvY - subSquareUVHeight);
						uvs->GetCoordinate(vertexIndex + 5)->Set(uvX, uvY - subSquareUVHeight);
					}
				}

				currentWidth += subSquareWidth;
				vertexIndex += 6;
			}

			currentWidth = initialWidth;
			currentHeight -= subSquareHeight;
		}

		subMesh->SetNormalsSmoothingThreshold(85);
		subMesh->SetBuildFaces(buildFaces);
		subMesh->SetCalculateNormals(calculateNormals);
		subMesh->SetCalculateTangents(calculateTangents);
		mesh->SetSubMesh(subMesh, 0);

		return mesh;
	}

	Mesh3DRef EngineUtility::CreateCubeMesh(StandardAttributeSet meshAttributes)
	{
		return CreateCubeMesh(meshAttributes, false);
	}

	Mesh3DRef EngineUtility::CreateCubeMesh(StandardAttributeSet meshAttributes, bool doCCW)
	{
		// force mesh to have at least positions and normals
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);

		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
		SubMesh3DRef subMesh = objectManager->CreateSubMesh3D(meshAttributes);
		subMesh->Init(36);

		Mesh3DRef mesh = objectManager->CreateMesh3D(1);
		mesh->Init();
		mesh->SetSubMesh(subMesh, 0);

		Point3Array * points = subMesh->GetPostions();
		UV2Array *uvs = NULL;

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

		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))
		{
			Color4Array * colors = subMesh->GetColors();

			for (Int32 i = 0; i < 36; i++)
			{
				colors->GetColor(i)->Set(1, 1, 1, 1);
			}
		}

		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))
		{
			uvs = subMesh->GetUVs0();

			// --- Cube UVs -------
			// cube front, triangle 1
			uvs->GetCoordinate(0)->Set(0, 0);
			uvs->GetCoordinate(1)->Set(1, 0);
			uvs->GetCoordinate(2)->Set(0, 1);

			// cube front, triangle 2
			uvs->GetCoordinate(3)->Set(1, 0);
			uvs->GetCoordinate(4)->Set(1, 1);
			uvs->GetCoordinate(5)->Set(0, 1);

			// cube right, triangle 1
			uvs->GetCoordinate(6)->Set(0, 0);
			uvs->GetCoordinate(7)->Set(1, 0);
			uvs->GetCoordinate(8)->Set(0, 1);

			// cube right, triangle 2
			uvs->GetCoordinate(9)->Set(1, 0);
			uvs->GetCoordinate(10)->Set(1, 1);
			uvs->GetCoordinate(11)->Set(0, 1);

			// cube left, triangle 1
			uvs->GetCoordinate(12)->Set(0, 0);
			uvs->GetCoordinate(13)->Set(1, 0);
			uvs->GetCoordinate(14)->Set(0, 1);

			// cube left, triangle 2
			uvs->GetCoordinate(15)->Set(1, 0);
			uvs->GetCoordinate(16)->Set(1, 1);
			uvs->GetCoordinate(17)->Set(0, 1);

			// cube top, triangle 1
			uvs->GetCoordinate(18)->Set(0, 0);
			uvs->GetCoordinate(19)->Set(1, 0);
			uvs->GetCoordinate(20)->Set(0, 1);

			// cube top, triangle 2
			uvs->GetCoordinate(21)->Set(1, 0);
			uvs->GetCoordinate(22)->Set(1, 1);
			uvs->GetCoordinate(23)->Set(0, 1);

			// cube back, triangle 1
			uvs->GetCoordinate(24)->Set(0, 0);
			uvs->GetCoordinate(25)->Set(1, 0);
			uvs->GetCoordinate(26)->Set(0, 1);

			// cube back, triangle 2
			uvs->GetCoordinate(27)->Set(1, 0);
			uvs->GetCoordinate(28)->Set(1, 1);
			uvs->GetCoordinate(29)->Set(0, 1);

			// cube back, triangle 1
			uvs->GetCoordinate(30)->Set(0, 0);
			uvs->GetCoordinate(31)->Set(1, 0);
			uvs->GetCoordinate(32)->Set(0, 1);

			// cube back, triangle 2
			uvs->GetCoordinate(33)->Set(1, 0);
			uvs->GetCoordinate(34)->Set(1, 1);
			uvs->GetCoordinate(35)->Set(0, 1);
		}

		if (doCCW)
		{
			for (UInt32 i = 0; i < 36; i += 3)
			{
				Point3 * p1 = points->GetPoint(i);
				Point3  p1r = *p1;
				Point3 * p3 = points->GetPoint(i + 2);

				*p1 = *p3;
				*p3 = p1r;

				UV2 * u1 = uvs->GetCoordinate(i);
				UV2  u1r = *u1;
				UV2 * u3 = uvs->GetCoordinate(i + 2);

				*u1 = *u3;
				*u3 = u1r;
			}
		}

		subMesh->SetNormalsSmoothingThreshold(85);
		mesh->Update();
		return mesh;
	}
}
