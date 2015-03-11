#include <iostream>
#include <math.h>

#include "camera.h"
#include "engine.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/render/rendertarget.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/graphics.h"
#include "graphics/screendesc.h"
#include "graphics/shader/shadersource.h"
#include "graphics/stdattributes.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "base/intmask.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"
#include "asset/assetimporter.h"
#include "util/engineutility.h"

Camera::Camera()
{
	clearBufferMask = 0;

	skyboxSetup = false;
	skyboxEnabled = false;

	ssaoMode = SSAORenderMode::Standard;
	ssaoEnabled = false;

	renderOrderIndex = 0;

	UpdateDisplay();
}

Camera::~Camera()
{

}

void Camera::SetupSkybox(TextureRef cubeTexture)
{
	ASSERT_RTRN(cubeTexture.IsValid(), "Camera::SetSkybox -> cube texture is not valid.");

	if(!skyboxSetup)
	{
		// instantiate an asset importer to load shaders
		AssetImporter importer;

		// get reference to the engine's object manager
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		skyboxSceneObject = objectManager->CreateSceneObject();
		ASSERT_RTRN(skyboxSceneObject.IsValid(), "Camera::SetSkybox -> Unable to create skybox scene object.");
		skyboxSceneObject->SetActive(false);

		ShaderSource skyboxShaderSource;
		importer.LoadBuiltInShaderSource("skybox", skyboxShaderSource);
		skyboxMaterial = objectManager->CreateMaterial(std::string("SkyBox"), skyboxShaderSource);
		ASSERT_RTRN(skyboxMaterial.IsValid(), "Camera::SetSkybox -> Unable to create skybox material.");

		skyboxMaterial->SetSelfLit(true);
		skyboxMaterial->SetTexture(cubeTexture, "SKYBOX_TEXTURE");

		StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
		skyboxMesh = EngineUtility::CreateCubeMesh(meshAttributes, true);
		ASSERT_RTRN(skyboxMesh.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh.");

		skyboxMeshFilter = objectManager->CreateMesh3DFilter();
		ASSERT_RTRN(skyboxMeshFilter.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh filter.");
		skyboxMeshFilter->SetMesh3D(skyboxMesh);
		skyboxSceneObject->SetMesh3DFilter(skyboxMeshFilter);

		skyboxMeshRenderer = objectManager->CreateMesh3DRenderer();
		ASSERT_RTRN(skyboxMeshRenderer.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh renderer.");
		skyboxMeshRenderer->AddMaterial(skyboxMaterial);
		skyboxSceneObject->SetMesh3DRenderer(skyboxMeshRenderer);

		skyboxSetup = true;
	}
}

void Camera::ShareSkybox(CameraRef camera)
{
	sharedSkyboxCamera = camera;
	skyboxSetup = true;
}

bool Camera::IsSkyboxSetup() const
{
	return skyboxSetup;
}

void Camera::SetSkyboxEnabled(bool enabled)
{
	skyboxEnabled = enabled;
}

bool Camera::IsSkyboxEnabled() const
{
	return skyboxEnabled;
}

SceneObjectRef Camera::GetSkyboxSceneObject()
{
	if(sharedSkyboxCamera.IsValid())return sharedSkyboxCamera->GetSkyboxSceneObject();
	return skyboxSceneObject;
}

void Camera::SetSSAOEnabled(bool enabled)
{
	ssaoEnabled = enabled;
}

bool Camera::IsSSAOEnabled()
{
	return ssaoEnabled;
}

void Camera::SetSSAORenderMode(SSAORenderMode mode)
{
	ssaoMode = mode;
}

SSAORenderMode Camera::GetSSAORenderMode()
{
	return ssaoMode;
}

void Camera::SetRendeOrderIndex(unsigned int index)
{
	renderOrderIndex = index;
}

unsigned int Camera::GetRendeOrderIndex()
{
	return renderOrderIndex;
}

void Camera::AddClearBuffer(RenderBufferType buffer)
{
	IntMaskUtil::SetBitForMask(&clearBufferMask, (unsigned int)buffer);
}

void Camera::RemoveClearBuffer(RenderBufferType buffer)
{
	IntMaskUtil::ClearBitForMask(&clearBufferMask, (unsigned int)buffer);
}

IntMask Camera::GetClearBufferMask() const
{
	return clearBufferMask;
}

const Transform& Camera::GetProjectionTransform() const
{
	return projection;
}

void Camera::TransformProjectionTransformBy(const Transform& transform)
{
	projection.TransformBy(transform);
}

void Camera::PreTransformProjectionTransformBy(const Transform& transform)
{
	projection.PreTransformBy(transform);
}

void Camera::SetupOffscreenRenderTarget(int width, int height)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	if(renderTarget.IsValid())
	{
		objectManager->DestroyRenderTarget(renderTarget);
	}

	TextureAttributes colorAttributes;
	colorAttributes.FilterMode = TextureFilter::Linear;
	colorAttributes.MipMapLevel = 8;
	colorAttributes.WrapMode = TextureWrap::Clamp;
	renderTarget = objectManager->CreateRenderTarget(true,true,colorAttributes,width, height);

	UpdateDisplay();
}

RenderTargetRef Camera::GetRenderTarget()
{
	if(!renderTarget.IsValid())return Engine::Instance()->GetGraphicsEngine()->GetDefaultRenderTarget();
	else return renderTarget;
}

void Camera::UpdateDisplay()
{
	Matrix4x4 proj;
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();

	float ratio = 1;

	if(renderTarget.IsValid())
	{
		ratio = (float)renderTarget->GetWidth() / (float)renderTarget->GetHeight();
	}
	else
	{
		RenderTargetRef defaultRenderTarget = graphics->GetDefaultRenderTarget();
		ratio = (float)defaultRenderTarget->GetWidth() / (float)defaultRenderTarget->GetHeight();
	}

	Transform::BuildProjectionMatrix(proj, 65, ratio, 5, 200);
	projection.SetTo(proj);
}

void Camera::SetCullingMask(IntMask mask)
{
	cullingMask = mask;
}

void Camera::MergeCullingMask(IntMask mask)
{
	cullingMask = IntMaskUtil::MergeMasks(cullingMask, mask);
}

IntMask Camera::GetCullingMask() const
{
	return cullingMask;
}

