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
#include "graphics/animation/vertexbonemap.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendertarget.h"
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

	walkAnimationSpeed = 2;
	walkSpeed = 6.0;
	runAnimationSpeed = 3;
	runSpeed = 6.0;
	rotateSpeed = 200;
	speedSmoothing = 10;

	basePlayerForward = Vector3(0,0,1);
	baseCameraForward = Vector3(0,0,-1);
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

Mesh3DRef Game::FindFirstMesh(SceneObjectRef ref)
{
	if(!ref.IsValid())return Mesh3DRef::Null();

	if(ref->GetMesh3D().IsValid())return ref->GetMesh3D();

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		Mesh3DRef subRef = FindFirstMesh(childRef);
		if(subRef.IsValid())return subRef;
	}

	return Mesh3DRef::Null();
}

void Game::Init()
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	Mesh3DRendererRef renderer;
	SceneObjectRef sceneObject;
	TextureAttributes texAttributes;
	TextureRef texture;
	Mesh3DRef mesh;
	StandardAttributeSet meshAttributes;
	SceneObjectRef childSceneObject;

	cameraObject = objectManager->CreateSceneObject();
	CameraRef camera = objectManager->CreateCamera();
	cameraObject->GetLocalTransform().Translate(0, 5, 25, true);
	// cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,90);
	camera->AddClearBuffer(RenderBufferType::Color);
	camera->AddClearBuffer(RenderBufferType::Depth);
	cameraObject->SetCamera(camera);


	cube = objectManager->CreateSceneObject();
	cube->GetLocalTransform().Scale(1.5, 1.5,1.5, true);
	//cube->GetLocalTransform().RotateAround(0, 0, 0, 0, 1, 0, 45);
	cube->GetLocalTransform().Translate(2, -7, 10, false);

	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	texture = objectManager->CreateTexture("textures/cartoonTex03.png", texAttributes);

	MaterialRef material = objectManager->CreateMaterial("BasicMaterial", "resources/basic.vertex.shader","resources/basic.fragment.shader");
	material->SetTexture(texture, "TEXTURE0");

    renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cube->SetMesh3DRenderer(renderer);

	meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	mesh = GameUtil::CreateCubeMesh(meshAttributes);
	cube->SetMesh3D(mesh);

	Mesh3DRef firstMesh = FindFirstMesh(cube);
	firstMesh->SetCastShadows(true);
	firstMesh->SetReceiveShadows(true);


	// ----- second cube ------
	/*childSceneObject = objectManager->CreateSceneObject();

	childSceneObject->SetMesh3D(mesh);
	childSceneObject->SetMesh3DRenderer(renderer);

	childSceneObject->GetLocalTransform().Translate(-2, 3, 0, true);
	childSceneObject->GetLocalTransform().Scale(1.5,1.5,1.5, true);
	//childSceneObject->GetTransform()->Translate(9, 0, 0, false);*/




	AssetImporter * importer = new AssetImporter();
	SceneObjectRef modelSceneObject;



	modelSceneObject = importer->LoadModelDirect("../../models/toonlevel/island/island.fbx", 1 );
	if(modelSceneObject.IsValid())
	{
		modelSceneObject->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}
	firstMesh = FindFirstMesh(modelSceneObject);
	//firstMesh->SetCastShadows(true);
	//firstMesh->SetReceiveShadows(true);
	modelSceneObject->GetLocalTransform().Translate(0,-10,0,false);
	modelSceneObject->GetLocalTransform().Scale(.07,.05,.07, true);



	modelSceneObject = importer->LoadModelDirect("../../models/toonlevel/castle/Tower_01.fbx", 1 );
	if(modelSceneObject.IsValid())
	{
		modelSceneObject->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}
	firstMesh = FindFirstMesh(modelSceneObject);
	//firstMesh->SetCastShadows(true);
	//firstMesh->SetReceiveShadows(true);
	modelSceneObject->GetLocalTransform().Translate(10,-10,-10,false);
	modelSceneObject->GetLocalTransform().Scale(.05,.05,.05, true);


	modelSceneObject = importer->LoadModelDirect("../../models/toonlevel/castle/MushRoom_01.fbx", 1 );
	if(modelSceneObject.IsValid())
	{
		modelSceneObject->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}
	firstMesh = FindFirstMesh(modelSceneObject);
	//firstMesh->SetCastShadows(true);
	//firstMesh->SetReceiveShadows(true);
	modelSceneObject->GetLocalTransform().Translate(-10,-10,20,false);
	modelSceneObject->GetLocalTransform().Scale(.09,.09,.09, true);





	SceneObjectRef defaultObject = importer->LoadModelDirect("../../models/cartoonnerd/DefaultAvatar/DefaultAvatar.fbx", 1 );
	SkinnedMesh3DRendererRef defaultMeshRenderer = FindFirstSkinnedMeshRenderer(defaultObject);
	SkeletonRef defaultSkeleton = defaultMeshRenderer->GetSkeleton();





	//playerObject = importer->LoadModelDirect("../../models/koopa/model/koopa.fbx", 1 );
	playerObject = importer->LoadModelDirect("../../models/cartoonnerd/cartoonnerd2.fbx", 1 );
	//playerObject = importer->LoadModelDirect("../../models/cartoonnerd/DefaultAvatar/DefaultAvatar.fbx", 1 );
	SkinnedMesh3DRendererRef playerMeshRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	SkeletonRef playerSkeleton = playerMeshRenderer->GetSkeleton();
	if(playerObject.IsValid())
	{
		playerObject->SetActive(true);
	}
	else
	{
		Debug::PrintError(" >> could not load model!\n");
		return;
	}



	/*defaultSkeleton->OverrideBonesFrom(playerMeshRenderer->GetSkeleton(), false, true);
	playerMeshRenderer->SetSkeleton(defaultSkeleton);
	for(unsigned int s =0; s < playerMeshRenderer->GetSubRendererCount(); s++)
	{
		playerMeshRenderer->GetVertexBoneMap(s)->BindTo(defaultSkeleton);
	}*/



	firstMesh = playerMeshRenderer->GetMesh();
	//firstMesh->SetCastShadows(true);
	//firstMesh->SetReceiveShadows(true);
	//playerObject->GetLocalTransform().RotateAround(0,0,0,1,0,0,45);
	//modelSceneObject->GetLocalTransform().RotateAround(0,0,0,0,1,0,-90);
	playerObject->GetLocalTransform().Translate(0,-10,-2,false);
	playerObject->GetLocalTransform().Scale(.15, .15, .15, true);

	playerWait = importer->LoadAnimation("../../models/cartoonnerd/human@idleneutral.fbx");
	playerWalk = importer->LoadAnimation("../../models/cartoonnerd/human@walk.fbx");
	//playerWait = importer->LoadAnimation("../../models/koopa/model/koopa@wait.fbx");
	//playerWalk = importer->LoadAnimation("../../models/koopa/model/koopa@walk.fbx");
	//playerJump = importer->LoadAnimation("../../models/koopa/model/koopa@jump.fbx");
	//playerRoar = importer->LoadAnimation("../../models/koopa/model/koopa@roar3.fbx");

	playerRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	AnimationManager * animManager = Engine::Instance()->GetAnimationManager();
	bool compatible = animManager->IsCompatible(playerRenderer, playerWalk);
	compatible &= animManager->IsCompatible(playerRenderer, playerWait);
	//compatible &= animManager->IsCompatible(playerRenderer, playerJump);
	//compatible &= animManager->IsCompatible(playerRenderer, playerRoar);

	if(compatible)printf("animation is compatible!! :)\n");
	else printf("animation is not compatible! boooo!\n");

	if(compatible)
	{
		animationPlayer = animManager->RetrieveOrCreateAnimationPlayer(playerRenderer);
		animationPlayer->AddAnimation(playerWait);
		animationPlayer->AddAnimation(playerWalk);

		//animationPlayer->AddAnimation(playerJump);
		//animationPlayer->AddAnimation(playerRoar);

		//animationPlayer->SetSpeed(playerWalk, 2);
		//animationPlayer->Play(playerWait);
	}






	SceneObjectRef lightObject;
	LightRef light;

	meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	mesh = GameUtil::CreateCubeMesh(meshAttributes);

	MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", "resources/builtin/selflit.vertex.shader","resources/builtin/selflit.fragment.shader");


	/*
	sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2.3);
	light->SetRange(45);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(0, 10, 30, false);





	sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2.3);
	light->SetRange(55);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(-2, 10, -10, false);




	sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	light->SetRange(45);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(-30, -3, 5, false);




	sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(2);
	light->SetRange(45);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(30, -3, 5, false);



	sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(3);
	light->SetRange(35);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(5, -30, 45, false);
*/



	/*sceneObject = objectManager->CreateSceneObject();
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	sceneObject->SetMesh3DRenderer(renderer);

	light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(1.7);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Point);
	sceneObject->SetLight(light);

	sceneObject->SetMesh3D(mesh);
	sceneObject->GetLocalTransform().Scale(.4,.4,.4, true);
	sceneObject->GetLocalTransform().Translate(5, 0, 20, false);*/


	sceneObject = objectManager->CreateSceneObject();
	light = objectManager->CreateLight();
	light->SetIntensity(.30);
	light->SetType(LightType::Ambient);
	sceneObject->SetLight(light);


	sceneObject = objectManager->CreateSceneObject();

	light = objectManager->CreateLight();
//	light->SetDirection(-.8,-1.7,-2);
	light->SetDirection(0,-1,0);
	light->SetIntensity(1.2);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Directional);
	sceneObject->SetLight(light);


	InitializePlayerPosition();
}

void Game::InitializePlayerPosition()
{
	SceneObjectTransform playerTransform;
	playerObject->InitSceneObjectTransform(&playerTransform);

	Vector3 playerForward = basePlayerForward;
	playerTransform.TransformVector(playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	lookDirection = playerForward;
}

void Game::Update()
{
	Point3 cameraPos;
	Point3 playerPos;

	SceneObjectTransform cameraTransform;
	cameraObject->InitSceneObjectTransform(&cameraTransform);
	cameraTransform.TransformPoint(cameraPos);

	 //cameraObject->GetLocalTransform().RotateAround(0,0,-12,0,1,0,12 * Time::GetDeltaTime() * rotationDir);

	 /*isWalking = true;
	 AnimationManager * animManager = AnimationManager::Instance();
	 AnimationPlayerRef player = animManager->RetrieveOrCreateAnimationPlayer(koopaRenderer);
	 player->CrossFade(playerWalk, 2);*/

	 //float realTime = Time::GetRealTimeSinceStartup();

	cube->GetLocalTransform().Rotate(0,1,0,20 * Time::GetDeltaTime());

	UpdatePlayerMovementDirection();
	//UpdatePlayerAnimation();
	UpdatePlayerPosition();
	UpdatePlayerLookDirection();
	UpdatePlayerFollowCamera();
}

void Game::UpdatePlayerMovementDirection()
{
	Point3 cameraPos;
	Point3 playerPos;

	SceneObjectTransform cameraTransform;
	cameraObject->InitSceneObjectTransform(&cameraTransform);
	cameraTransform.TransformPoint(cameraPos);

	SceneObjectTransform playerTransform;
	playerObject->InitSceneObjectTransform(&playerTransform);
	playerTransform.TransformPoint(playerPos);

	Vector3 playerForward = basePlayerForward;
	playerTransform.TransformVector(playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	Vector3 cameraForward = baseCameraForward;
	cameraTransform.TransformVector(cameraForward);
	cameraForward.y = 0;
	cameraForward.Normalize();

	Vector3 cameraRight;
	Vector3::Cross(cameraForward, Vector3::Up, cameraRight);

	float h = 0;
	float v = 0;
	InputManager * inputManager = Engine::Instance()->GetInputManager();

	if(inputManager->GetDigitalInputState(DigitalInput::Left))h -= 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Right))h += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Up))v += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Down))v -= 1;

	isMoving = GTEMath::Abs(h) > .1 || GTEMath::Abs(v) > .1;

	Vector3 cameraRightScaled = cameraRight;
	cameraRightScaled.Scale(h);

	Vector3 cameraForwardScaled = cameraForward;
	cameraForwardScaled.Scale(v);

	Vector3 targetDirection;
	Vector3::Add(cameraRightScaled, cameraForwardScaled, targetDirection);
	targetDirection.Normalize();


	if(targetDirection.x != 0 || targetDirection.y != 0 || targetDirection.z != 0)
	{
		//float dot = Vector3::Dot(&lookDirection, &targetDirection);

		bool success = Vector3::RotateTowards(lookDirection, targetDirection,  rotateSpeed * Time::GetDeltaTime(), moveDirection);

		if(!success)
		{
			Vector3::Cross(Vector3::Up, lookDirection, targetDirection);
			Vector3::RotateTowards(lookDirection, targetDirection,  rotateSpeed * Time::GetDeltaTime(), moveDirection);
		}


		lookDirection = moveDirection;
	}

	if(isGrounded)
	{
		float targetSpeed = 0;
		float curSmooth = speedSmoothing * Time::GetDeltaTime();
		if(isMoving)
		{
			targetSpeed = walkSpeed;
		}
		moveSpeed = GTEMath::Lerp(moveSpeed, targetSpeed, curSmooth);
	}
	else
	{

	}
}

void Game::UpdatePlayerPosition()
{
	if(moveSpeed > .1)
	{
		Vector3 move = lookDirection;
		move.Scale(moveSpeed * Time::GetDeltaTime() );
		playerObject->GetLocalTransform().Translate(move, false);
	}
}

void Game::UpdatePlayerLookDirection()
{
//	if(lookDirection.SquareMagnitude() > .00001)
	//{
		SceneObjectTransform playerTransform;
		playerObject->InitSceneObjectTransform(&playerTransform);

		Vector3 playerForward = basePlayerForward;
		playerTransform.TransformVector(playerForward);
		playerForward.y = 0;
		playerForward.Normalize();

		Vector3 localLookDirection = lookDirection;
		Matrix4x4 inversePlayerTransform;
		playerTransform.CopyMatrix(inversePlayerTransform);
		inversePlayerTransform.Invert();
		inversePlayerTransform.Transform(localLookDirection);

		Vector3 localAxis(0,1,0);
		inversePlayerTransform.Transform(localAxis);

		lookDirection.y = 0;
		lookDirection.Normalize();

		//float diff = Vector3::Dot(&playerForward, &lookDirection);
		//if(diff >= .9999)return;

		Quaternion modRotation = Quaternion::getRotation(basePlayerForward, lookDirection, localAxis);
		modRotation.normalize();

		Quaternion currentRotation;
		Vector3 currentTranslation;
		Vector3 currentScale;

		playerTransform.GetLocalComponents(currentTranslation, currentRotation, currentScale);
		playerTransform.SetLocalComponents(currentTranslation, modRotation, currentScale);
	//}
}

void Game::UpdatePlayerAnimation()
{
	if(moveSpeed > .1)
	{
		animationPlayer->CrossFade(playerWalk, .2);
	}
	else
	{
		animationPlayer->CrossFade(playerWait, .3);
	}
}

void Game::UpdatePlayerFollowCamera()
{
	Point3 cameraPos;
	Point3 playerPos;
	Point3 playerPosCameraLookTarget;
	Point3 playerPosCameraMoveTarget;
	Vector3 cameraForward;

	SceneObjectTransform cameraTransform;
	cameraObject->InitSceneObjectTransform(&cameraTransform);
	cameraTransform.TransformPoint(cameraPos);
	cameraForward = baseCameraForward;
	cameraTransform.TransformVector(cameraForward);
	cameraForward.Normalize();

	SceneObjectTransform playerTransform;
	playerObject->InitSceneObjectTransform(&playerTransform);
	playerTransform.TransformPoint(playerPos);

	playerPosCameraLookTarget = playerPos;
	playerPosCameraMoveTarget = playerPos;

	playerPosCameraLookTarget.y = cameraPos.y;
	playerPosCameraMoveTarget.y = playerPos.y + 10;

	Vector3 cameraToPlayerLook;
	Point3::Subtract(playerPosCameraLookTarget, cameraPos, cameraToPlayerLook);

	Vector3 cameraToPlayerMove;
	Point3::Subtract(playerPosCameraMoveTarget, cameraPos, cameraToPlayerMove);
	cameraToPlayerMove.y=0;

	float desiredFollowDistance = 30;
	float currentDistance = cameraToPlayerMove.Magnitude();

	float scaleFactor = desiredFollowDistance;
	if(currentDistance != 0)scaleFactor = desiredFollowDistance/currentDistance;

	Vector3 newCameraToPlayer = cameraToPlayerMove;
	newCameraToPlayer.Scale(scaleFactor);
	newCameraToPlayer.Invert();

	Point3 cameraTarget;
	Point3::Add(playerPosCameraMoveTarget, newCameraToPlayer, cameraTarget);

	Point3 newCameraPos;
	Point3::Lerp(cameraPos, cameraTarget, newCameraPos, .4 * Time::GetDeltaTime());

	Vector3 cameraMove;
	Point3::Subtract(newCameraPos, cameraPos, cameraMove);


	cameraToPlayerMove.Normalize();

	Quaternion cameraRotationA;
	cameraRotationA = Quaternion::getRotation(baseCameraForward,cameraToPlayerLook);
	cameraRotationA.normalize();

	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	cameraTransform.GetLocalComponents(currentTranslation, currentRotation, currentScale);
	cameraTransform.SetLocalComponents(currentTranslation, cameraRotationA, currentScale);

	cameraObject->GetLocalTransform().Translate(cameraMove, false);


}
