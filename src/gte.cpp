#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <memory>
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "asset/assetimporter.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/render/material.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
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
#include "object/enginetypes.h"
#include "util/time.h"
#include "gte.h"

class CustomGraphicsCallbacks: public GraphicsCallbacks
{
	public:

	SceneObjectRef cameraObject;
	CustomGraphicsCallbacks()
	{

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
		Point3 test;
		test.x = 5;
		test.y = 5;
		test.x = 5;


		EngineObjectManager * objectManager = EngineObjectManager::Instance();

		cameraObject = objectManager->CreateSceneObject();
		CameraRef camera = objectManager->CreateCamera();
		cameraObject->GetLocalTransform()->Translate(0, 5, 15, true);
		// cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,90);
		camera->AddClearBuffer(RenderBufferType::Color);
		camera->AddClearBuffer(RenderBufferType::Depth);
		cameraObject->SetCamera(camera);


		SceneObjectRef sceneObject = objectManager->CreateSceneObject();
		sceneObject->GetLocalTransform()->Scale(3,3,3, true);
		sceneObject->GetLocalTransform()->RotateAround(0, 0, 0, 0, 1, 0, 45);
		sceneObject->GetLocalTransform()->Translate(-15, 0, -12, false);

		TextureAttributes texAttributes;
		texAttributes.FilterMode = TextureFilter::TriLinear;
		texAttributes.MipMapLevel = 4;
		Texture * texture = objectManager->CreateTexture("textures/cartoonTex03.png", texAttributes);

		MaterialRef material = objectManager->CreateMaterial("BasicMaterial", "resources/basic.vertex.shader","resources/basic.fragment.shader");
		material->SetTexture(texture, "TEXTURE0");

		Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
		renderer->AddMaterial(material);
		sceneObject->SetMeshRenderer3D(renderer);

		StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);




		SubMesh3DRef subMesh = objectManager->CreateSubMesh3D(meshAttributes);
		subMesh->Init(36);

		Mesh3DRef mesh = objectManager->CreateMesh3D(1);
		mesh->Init();
		mesh->SetSubMesh(subMesh, 0);

		Point3Array * points = subMesh->GetPostions();
		Color4Array * colors = subMesh->GetColors();
		UV2Array *uvs = subMesh->GetUVsTexture0();

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

		for(int i=0; i< 36; i++)
		{
			colors->GetColor(i)->Set(1,1,1,1);
		}

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

		subMesh->SetNormalsSmoothingThreshold(85);
		subMesh->Update();
		sceneObject->SetMesh3D(mesh);

		SceneObjectRef childSceneObject = objectManager->CreateSceneObject();
		sceneObject->AddChild(childSceneObject);

		childSceneObject->SetMesh3D(mesh);
		childSceneObject->SetMeshRenderer3D(renderer);

		childSceneObject->GetLocalTransform()->Translate(-2, 3, 0, true);
		childSceneObject->GetLocalTransform()->Scale(1.5,1.5,1.5, true);
		//childSceneObject->GetTransform()->Translate(9, 0, 0, false);



		AssetImporter * importer = new AssetImporter();
		SceneObjectRef modelSceneObject;

		modelSceneObject = importer->LoadModelDirect("../../models/houseA/houseA_obj.obj", 1 );

		if(modelSceneObject.IsValid())
		{
			modelSceneObject->SetActive(true);
		}
		else
		{
			Debug::PrintError(" >> could not load model!\n");
			return;
		}
		modelSceneObject->GetLocalTransform()->RotateAround(0,0,0,0,1,0, 90);
		modelSceneObject->GetLocalTransform()->Translate(10,0,-12,false);
		modelSceneObject->GetLocalTransform()->Scale(.10,.10,.10, true);


		modelSceneObject = importer->LoadModelDirect("../../models/Rck-Wtrfll_dae/Rck-Wtrfll_dae.dae", 1 );
		if(modelSceneObject.IsValid())
		{
			modelSceneObject->SetActive(true);
		}
		else
		{
			Debug::PrintError(" >> could not load model!\n");
			return;
		}
		modelSceneObject->GetLocalTransform()->Translate(0,0,-12,false);
		modelSceneObject->GetLocalTransform()->Scale(.15,.15,.15, true);



		modelSceneObject = importer->LoadModelDirect("../../models/koopa/model/koopa.fbx", 1 );
		if(modelSceneObject.IsValid())
		{
			modelSceneObject->SetActive(true);
		}
		else
		{
			Debug::PrintError(" >> could not load model!\n");
			return;
		}
		modelSceneObject->GetLocalTransform()->RotateAround(0,0,0,1,0,0,-90);
		modelSceneObject->GetLocalTransform()->RotateAround(0,0,0,0,1,0,-90);
		modelSceneObject->GetLocalTransform()->Translate(0,-8,-3,false);
		modelSceneObject->GetLocalTransform()->Scale(.15,.15,.15, true);

		SceneObjectRef lightObject;
		LightRef light;

		lightObject = objectManager->CreateSceneObject();
		lightObject->GetLocalTransform()->Translate(0, 25, 15, false);
		light = objectManager->CreateLight();
		light->SetDirection(1,-1,-1);
		light->SetIntensity(2);
		light->SetRange(30);
		lightObject->SetLight(light);

		lightObject = objectManager->CreateSceneObject();
		lightObject->GetLocalTransform()->Translate(0, 15, -10, false);
		light = objectManager->CreateLight();
		light->SetDirection(1,-1,-1);
		light->SetIntensity(2);
		lightObject->SetLight(light);

		lightObject = objectManager->CreateSceneObject();
		lightObject->GetLocalTransform()->Translate(-15, -3, 5, false);
		light = objectManager->CreateLight();
		light->SetDirection(1,-1,-1);
		light->SetIntensity(2);
		lightObject->SetLight(light);

		lightObject = objectManager->CreateSceneObject();
		lightObject->GetLocalTransform()->Translate(15, -3, 5, false);
		light = objectManager->CreateLight();
		light->SetDirection(1,-1,-1);
		light->SetIntensity(2);
		lightObject->SetLight(light);
	}

	void OnUpdate(Graphics * graphics)
	{
		 cameraObject->GetLocalTransform()->RotateAround(0,0,-12,0,1,0,45 * Time::GetDeltaTime());
		 //printf("total time: %f\n", Time::GetRealTimeSinceStartup());
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
	bool initSuccess = Graphics::Instance()->Init(1280, 800,(CustomGraphicsCallbacks*) (&graphicsCallbacks), "GTE Test");
	if(initSuccess)
	{
		Graphics::Instance()->Run();
	}
	else
	{
		Debug::PrintError("Error occurred while initializing engine.");
	}

	return EXIT_SUCCESS;
}

