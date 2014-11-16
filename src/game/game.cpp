#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <memory>
#include "game.h"
#include "engine.h"
#include "input/inputmanager.h"
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
#include "base/basevector4.h"
#include "geometry/matrix4x4.h"
#include "geometry/quaternion.h"
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
#include "global/global.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"


Game::Game()
{
	moveSpeed = 0;
	isMoving = false;
	isGrounded = true;

	walkSpeed = 3.0;
	runSpeed = 6.0;
	rotateSpeed = 50;
	speedSmoothing = 10;

	baseForward = Vector3(0,0,1);
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
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

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

	playerObject = importer->LoadModelDirect("../../models/koopa/model/koopa@wait.fbx", 1 );
	if(playerObject.IsValid())
	{
		playerObject->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}
	playerObject->GetLocalTransform().RotateAround(0,0,0,1,0,0,45);
	//modelSceneObject->GetLocalTransform().RotateAround(0,0,0,0,1,0,-90);
	playerObject->GetLocalTransform().Translate(0,-2,-2,false);
	playerObject->GetLocalTransform().Scale(.35, .35, .35, true);


	koopaWait = importer->LoadAnimation("../../models/koopa/model/koopa@wait.fbx");
	koopaWalk = importer->LoadAnimation("../../models/koopa/model/koopa@walk.fbx");
	koopaJump = importer->LoadAnimation("../../models/koopa/model/koopa@jump.fbx");
	koopaRoar = importer->LoadAnimation("../../models/koopa/model/koopa@roar3.fbx");

	koopaRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	AnimationManager * animManager = Engine::Instance()->GetAnimationManager();
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
		player->Play(koopaWalk);
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

	InitializePlayerPosition();
}

void Game::InitializePlayerPosition()
{
	SceneObjectTransform playerTransform;
	playerObject->GetFullTransform(&playerTransform);

	Vector3 playerForward = baseForward;
	playerTransform.TransformVector(&playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	lookDirection = playerForward;
}

void Game::Update()
{
	Point3 cameraPos;
	Point3 playerPos;

	SceneObjectTransform cameraTransform;
	cameraObject->GetFullTransform(&cameraTransform);
	cameraTransform.GetMatrix()->Transform(&cameraPos);

	 //cameraObject->GetLocalTransform().RotateAround(0,0,-12,0,1,0,12 * Time::GetDeltaTime() * rotationDir);

	 /*isWalking = true;
	 AnimationManager * animManager = AnimationManager::Instance();
	 AnimationPlayerRef player = animManager->RetrieveOrCreateAnimationPlayer(koopaRenderer);
	 player->CrossFade(koopaWalk, 2);*/

	 //float realTime = Time::GetRealTimeSinceStartup();

	UpdatePlayerMovementDirection();
	UpdateLookDirection();
}

void Game::UpdatePlayerMovementDirection()
{
	Point3 cameraPos;
	Point3 playerPos;

	SceneObjectTransform cameraTransform;
	cameraObject->GetFullTransform(&cameraTransform);
	cameraTransform.GetMatrix()->Transform(&cameraPos);

	SceneObjectTransform playerTransform;
	playerObject->GetFullTransform(&playerTransform);
	playerTransform.TransformPoint(&playerPos);

	Vector3 playerForward = baseForward;
	playerTransform.TransformVector(&playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	Vector3 cameraForward = baseForward;
	cameraTransform.TransformVector(&cameraForward);
	cameraForward.y = 0;
	cameraForward.Normalize();

	Vector3 cameraRight;
	cameraRight.Set(-cameraForward.z, 0, cameraForward.x);

	float h = 0;
	float v = 0;
	InputManager * inputManager = Engine::Instance()->GetInputManager();

	if(inputManager->GetDigitalInputState(DigitalInput::Left))h -= 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Right))h += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Up))v += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Down))v -= 1;

	bool wasMoving = isMoving;
	isMoving = GTEMath::Abs(h) > .1 || GTEMath::Abs(v) > .1;

	Vector3 cameraRightScaled = cameraRight;
	cameraRightScaled.Scale(h);

	Vector3 cameraForwardScaled = cameraForward;
	cameraForwardScaled.Scale(v);

	Vector3 targetDirection;
	Vector3::Add(&cameraRightScaled, &cameraForwardScaled, &targetDirection);

	if(Vector3::Dot(&targetDirection, &playerForward) >= .999)
	{
		lookDirection = targetDirection;
	}
	else if(targetDirection.x != 0 || targetDirection.y != 0 || targetDirection.z != 0)
	{
		Vector3::RotateTowards(&lookDirection, &targetDirection, rotateSpeed * Time::GetDeltaTime(), &moveDirection);
		lookDirection = moveDirection;
	}

	if(isGrounded)
	{
		float curSmooth = speedSmoothing * Time::GetDeltaTime();

		float targetSpeed = walkSpeed;

		moveSpeed = GTEMath::Lerp(moveSpeed, targetSpeed, curSmooth);
	}
	else
	{

	}
}

void Game::UpdateLookDirection()
{
	if(lookDirection.SquareMagnitude() > .001)
	{
		SceneObjectTransform playerTransform;
		playerObject->GetFullTransform(&playerTransform);

		Vector3 playerForward = baseForward;
		playerTransform.TransformVector(&playerForward);
		playerForward.y = 0;
		playerForward.Normalize();

		Vector3 localLookDirection = lookDirection;
		Matrix4x4 inversePlayerTransform;
		inversePlayerTransform.SetTo(playerTransform.GetMatrix());
		inversePlayerTransform.Invert();
		inversePlayerTransform.Transform(&localLookDirection);

		lookDirection.y = 0;
		lookDirection.Normalize();

		float diff = Vector3::Dot(&playerForward, &lookDirection);
		if(diff >= .9999)return;

		Quaternion rotation = Quaternion::getRotation(baseForward, localLookDirection);
		rotation.normalize();
		Matrix4x4 rotMatrix = rotation.rotationMatrix();

		playerObject->GetLocalTransform().TransformBy(&rotMatrix);
	}
}
