#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <memory>
#include "game.h"
#include "gameutil.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "graphics/animation/bone.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/render/material.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "ui/debug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "util/time.h"


Game::Game()
{
	offset = 0;
	boneIndex = -1;
	isWalking = false;
	isJumping = false;
	rotationDir = 1;
}

Game::~Game()
{

}

SkinnedMesh3DRendererRef Game::FindFirstSkinnedMeshRenderer(SceneObjectRef ref)
{
	if(!ref.IsValid())return SkinnedMesh3DRendererRef::Null();

	if(ref->GetSkinnedMesh3DRenderer().IsValid())return ref->GetSkinnedMesh3DRenderer();

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		SkinnedMesh3DRendererRef subRef = FindFirstSkinnedMeshRenderer(childRef);
		if(subRef.IsValid())return subRef;
	}

	return SkinnedMesh3DRendererRef::Null();
}

void Game::Init()
{
	EngineObjectManager * objectManager = EngineObjectManager::Instance();

	cameraObject = objectManager->CreateSceneObject();
	CameraRef camera = objectManager->CreateCamera();
	cameraObject->GetLocalTransform().Translate(0, 5, 15, true);
	// cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,90);
	camera->AddClearBuffer(RenderBufferType::Color);
	camera->AddClearBuffer(RenderBufferType::Depth);
	cameraObject->SetCamera(camera);


	SceneObjectRef sceneObject = objectManager->CreateSceneObject();
	sceneObject->GetLocalTransform().Scale(3,3,3, true);
	sceneObject->GetLocalTransform().RotateAround(0, 0, 0, 0, 1, 0, 45);
	sceneObject->GetLocalTransform().Translate(-15, 0, -12, false);

	TextureAttributes texAttributes;
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	TextureRef texture = objectManager->CreateTexture("textures/cartoonTex03.png", texAttributes);

	MaterialRef material = objectManager->CreateMaterial("BasicMaterial", "resources/basic.vertex.shader","resources/basic.fragment.shader");
	material->SetTexture(texture, "TEXTURE0");

	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	sceneObject->SetMesh3DRenderer(renderer);

	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	Mesh3DRef mesh = GameUtil::CreateCubeMesh(meshAttributes);
	sceneObject->SetMesh3D(mesh);

	SceneObjectRef childSceneObject = objectManager->CreateSceneObject();
	sceneObject->AddChild(childSceneObject);

	childSceneObject->SetMesh3D(mesh);
	childSceneObject->SetMesh3DRenderer(renderer);

	childSceneObject->GetLocalTransform().Translate(-2, 3, 0, true);
	childSceneObject->GetLocalTransform().Scale(1.5,1.5,1.5, true);
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
	modelSceneObject->GetLocalTransform().RotateAround(0,0,0,0,1,0, 90);
	modelSceneObject->GetLocalTransform().Translate(10,0,-12,false);
	modelSceneObject->GetLocalTransform().Scale(.10,.10,.10, true);



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
	modelSceneObject->GetLocalTransform().Translate(0,0,-12,false);
	modelSceneObject->GetLocalTransform().Scale(13,13,13, true);

	koopaRoot = importer->LoadModelDirect("../../models/koopa/model/koopa.fbx", 1 );
	if(koopaRoot.IsValid())
	{
		koopaRoot->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}
	koopaRoot->GetLocalTransform().RotateAround(0,0,0,1,0,0,45);
	//modelSceneObject->GetLocalTransform().RotateAround(0,0,0,0,1,0,-90);
	koopaRoot->GetLocalTransform().Translate(0,-2,-2,false);
	koopaRoot->GetLocalTransform().Scale(.35, .35, .35, true);


	koopaWait = importer->LoadAnimation("../../models/koopa/model/koopa@wait.fbx");
	koopaWalk = importer->LoadAnimation("../../models/koopa/model/koopa@walk.fbx");
	koopaJump = importer->LoadAnimation("../../models/koopa/model/koopa@jump.fbx");
	koopaRoar = importer->LoadAnimation("../../models/koopa/model/koopa@roar3.fbx");

	koopaRenderer = FindFirstSkinnedMeshRenderer(koopaRoot);
	AnimationManager * animManager = AnimationManager::Instance();
	bool compatible = animManager->IsCompatible(koopaRenderer, koopaWalk);
	compatible &= animManager->IsCompatible(koopaRenderer, koopaWait);
	compatible &= animManager->IsCompatible(koopaRenderer, koopaJump);
	compatible &= animManager->IsCompatible(koopaRenderer, koopaRoar);

	if(compatible)printf("animation is compatible!! :)\n");
	else printf("animation is not compatible! boooo!\n");

	if(compatible)
	{
		AnimationPlayerRef player = animManager->RetrieveOrCreateAnimationPlayer(koopaRenderer);
		player->AddAnimation(koopaWait);
		player->AddAnimation(koopaWalk);
		player->AddAnimation(koopaJump);
		player->AddAnimation(koopaRoar);
		player->Play(koopaWait);
	}




	SceneObjectRef lightObject;
	LightRef light;

	lightObject = objectManager->CreateSceneObject();
	lightObject->GetLocalTransform().Translate(0, 25, 15, false);
	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	light->SetRange(30);
	lightObject->SetLight(light);

	lightObject = objectManager->CreateSceneObject();
	lightObject->GetLocalTransform().Translate(0, 15, -10, false);
	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	lightObject->SetLight(light);

	lightObject = objectManager->CreateSceneObject();
	lightObject->GetLocalTransform().Translate(-15, -3, 5, false);
	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	lightObject->SetLight(light);

	lightObject = objectManager->CreateSceneObject();
	lightObject->GetLocalTransform().Translate(15, -3, 5, false);
	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	lightObject->SetLight(light);
}

void Game::Update()
{
	Point3 cameraPos;
	SceneObjectTransform sot;
	cameraObject->GetFullTransform(&sot);
	sot.GetMatrix()->Transform(&cameraPos);

	if(rotationDir > 0)
	{
		if(cameraPos.x > 20)
		{
			rotationDir = -1;
		}
	}
	else
	{
		if(cameraPos.x < - 20)
		{
			rotationDir = 1;
		}
	}

	 cameraObject->GetLocalTransform().RotateAround(0,0,-12,0,1,0,12 * Time::GetDeltaTime() * rotationDir);

	 /*isWalking = true;
	 AnimationManager * animManager = AnimationManager::Instance();
	 AnimationPlayerRef player = animManager->RetrieveOrCreateAnimationPlayer(koopaRenderer);
	 player->CrossFade(koopaWalk, 2);*/

	 //float realTime = Time::GetRealTimeSinceStartup();
}

void Game::ControlPlayer()
{

}
