#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics/graphics.h"
#include "graphics/attributes.h"
#include "graphics/object/mesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/render/material.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "ui/debug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "gte.h"

class CustomGraphicsCallbacks: public GraphicsCallbacks
{
	public:

	SceneObject * cameraObject;
	CustomGraphicsCallbacks()
	{
		cameraObject = NULL;
	}

	void PrintMatrix(Matrix4x4 *m)
	{
		const float * data = m->GetDataPtr();
		for (int r = 0; r < 4; r++)
		{
			printf("[");
			for (int c = 0; c < 4; c++)
			{
				if (c > 0)
					printf(",");
				printf("%f", data[c * 4 + r]);
			}
			printf("]\n");
		}
	}

	void PrintVector(BaseVector4 * vector)
	{
		float * data = vector->GetDataPtr();
		printf("[");
		for (int c = 0; c < 4; c++)
		{
			if (c > 0)
				printf(",");
			printf("%f", data[c]);
		}
		printf("]\n");
	}

	void OnInit(Graphics * graphics)
	{
		EngineObjectManager * objectManager = EngineObjectManager::Instance();

		AttributeSet meshAttributes = Attributes::CreateAttributeSet();
		Attributes::AddAttribute(&meshAttributes, Attribute::Position);
		Attributes::AddAttribute(&meshAttributes, Attribute::UV1);
		Attributes::AddAttribute(&meshAttributes, Attribute::Color);

		cameraObject = objectManager->CreateSceneObject();
		Camera * camera = objectManager->CreateCamera();
		cameraObject->GetTransform()->Translate(0, 5, 5, true);
		// cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,90);
		camera->AddClearBuffer(RenderBufferType::Color);
		camera->AddClearBuffer(RenderBufferType::Depth);
		cameraObject->SetCamera(camera);

		SceneObject * sceneObject = objectManager->CreateSceneObject();
		sceneObject->GetTransform()->Scale(3,3,3, true);
		sceneObject->GetTransform()->RotateAround(0, 0, 0, 0, 1, 0, 45);
		sceneObject->GetTransform()->Translate(0, 0, -12, false);

		TextureAttributes texAttributes;
		Texture * texture = objectManager->CreateTexture("textures/cartoonTex03.png", texAttributes);
		Material * material = objectManager->CreateMaterial("BasicMaterial", "resources/basic.vertex.shader","resources/basic.fragment.shader");
		material->AddTexture(texture, "Texture");
		Mesh3D * mesh = objectManager->CreateMesh3D(meshAttributes);
		Mesh3DRenderer * meshRenderer = objectManager->CreateMesh3DRenderer();
		meshRenderer->UseMaterial(material);
		sceneObject->SetMesh(mesh);
		sceneObject->SetMeshRenderer(meshRenderer);

		mesh->Init(36);

		Point3Array * points = mesh->GetPostions();
		Color4Array * colors = mesh->GetColors();
		UV2Array *uvs = mesh->GetUVs1();

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



		// --- Cube colors -------
		// cube front, triangle 1
		colors->GetColor(0)->Set(1, 0, 0, 1);
		colors->GetColor(1)->Set(1, 0, 0, 1);
		colors->GetColor(2)->Set(1, 0, 0, 1);

		// cube front, triangle 2
		colors->GetColor(3)->Set(1, 0, 0, 1);
		colors->GetColor(4)->Set(1, 0, 0, 1);
		colors->GetColor(5)->Set(1, 0, 0, 1);

		// cube right, triangle 1
		colors->GetColor(6)->Set(1, .75, 0, 1);
		colors->GetColor(7)->Set(1, .75, 0, 1);
		colors->GetColor(8)->Set(1, .75, 0, 1);

		// cube right, triangle 2
		colors->GetColor(9)->Set(1, .75, 0, 1);
		colors->GetColor(10)->Set(1, .75, 0, 1);
		colors->GetColor(11)->Set(1, .75, 0, 1);

		// cube left, triangle 1
		colors->GetColor(12)->Set(1, 1, 0, 1);
		colors->GetColor(13)->Set(1, 1, 0, 1);
		colors->GetColor(14)->Set(1, 1, 0, 1);

		// cube left, triangle 2
		colors->GetColor(15)->Set(1, 1, 0, 1);
		colors->GetColor(16)->Set(1, 1, 0, 1);
		colors->GetColor(17)->Set(1, 1, 0, 1);

		// cube top, triangle 1
		colors->GetColor(18)->Set(0, 0, 1, 1);
		colors->GetColor(19)->Set(0, 0, 1, 1);
		colors->GetColor(20)->Set(0, 0, 1, 1);

		// cube top, triangle 2
		colors->GetColor(21)->Set(0, 0, 1, 1);
		colors->GetColor(22)->Set(0, 0, 1, 1);
		colors->GetColor(23)->Set(0, 0, 1, 1);

		// cube back, triangle 1
		colors->GetColor(24)->Set(0, 1, 0, 1);
		colors->GetColor(25)->Set(0, 1, 0, 1);
		colors->GetColor(26)->Set(0, 1, 0, 1);

		// cube back, triangle 2
		colors->GetColor(27)->Set(0, 1, 0, 1);
		colors->GetColor(28)->Set(0, 1, 0, 1);
		colors->GetColor(29)->Set(0, 1, 0, 1);



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
		uvs->GetCoordinate(29)->Set(0,1);



		meshRenderer->UpdateFromMesh();

		SceneObject * childSceneObject = objectManager->CreateSceneObject();
		sceneObject->AddChild(childSceneObject);
		childSceneObject->SetMesh(mesh);
		childSceneObject->SetMeshRenderer(meshRenderer);
		childSceneObject->GetTransform()->Scale(3,3,3, true);
		childSceneObject->GetTransform()->Translate(5, 2, 0, true);
		childSceneObject->GetTransform()->Translate(9, 0, 0, false);
	}

	void OnUpdate(Graphics * graphics)
	{
		 cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,1);
	}

	void OnQuit(Graphics * graphics)
	{

	}

	virtual ~CustomGraphicsCallbacks()
	{

	}
};

int main(int argc, char** argv)
{
	CustomGraphicsCallbacks graphicsCallbacks;
	Graphics::Instance()->Init(640, 480,
			(CustomGraphicsCallbacks*) (&graphicsCallbacks), "GTE Test");

	return EXIT_SUCCESS;
}

