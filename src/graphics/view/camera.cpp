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
#include "global/constants.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"
#include "asset/assetimporter.h"
#include "util/engineutility.h"

namespace GTE
{
	Camera::Camera()
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		const GraphicsAttributes& graphicsAttr = graphics->GetAttributes();

		clearBufferMask = 0;

		skyboxSetup = false;
		skyboxEnabled = false;

		ssaoMode = SSAORenderMode::Standard;
		ssaoEnabled = false;

		renderOrderIndex = 0;

		fov = 65;
		widthHeightRatio = (Real)graphicsAttr.WindowWidth / (Real)graphicsAttr.WindowHeight;

		clipPlaneCount = 0;

		reverseCulling = false;

		projectionMode = ProjectionMode::Perspective;

		UpdateDisplay();
	}

	Camera::~Camera()
	{

	}

	void Camera::SetupSkybox(TextureRef cubeTexture)
	{
		NONFATAL_ASSERT(cubeTexture.IsValid(), "Camera::SetSkybox -> 'cubeTexture' is not valid.", true);

		if (!skyboxSetup)
		{
			skyboxTexture = cubeTexture;

			// instantiate an asset importer to load shaders
			AssetImporter importer;

			// get reference to the engine's object manager
			EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

			skyboxSceneObject = objectManager->CreateSceneObject();
			ASSERT(skyboxSceneObject.IsValid(), "Camera::SetSkybox -> Unable to create skybox scene object.");
			skyboxSceneObject->SetActive(false);

			ShaderSource skyboxShaderSource;
			importer.LoadBuiltInShaderSource("skybox", skyboxShaderSource);
			skyboxMaterial = objectManager->CreateMaterial(std::string("SkyBox"), skyboxShaderSource);
			NONFATAL_ASSERT(skyboxMaterial.IsValid(), "Camera::SetSkybox -> Unable to create skybox material.", true);

			skyboxMaterial->SetSelfLit(true);
			skyboxMaterial->SetTexture(skyboxTexture, "SKYBOX_TEXTURE");

			StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
			StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
			StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
			StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
			skyboxMesh = EngineUtility::CreateCubeMesh(meshAttributes, true);
			ASSERT(skyboxMesh.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh.");

			skyboxMeshFilter = objectManager->CreateMesh3DFilter();
			ASSERT(skyboxMeshFilter.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh filter.");
			skyboxMeshFilter->SetMesh3D(skyboxMesh);
			skyboxSceneObject->SetMesh3DFilter(skyboxMeshFilter);

			skyboxMeshRenderer = objectManager->CreateMesh3DRenderer();
			ASSERT(skyboxMeshRenderer.IsValid(), "Camera::SetSkybox -> Unable to create skybox mesh renderer.");
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

	Bool Camera::IsSkyboxSetup() const
	{
		return skyboxSetup;
	}

	void Camera::SetSkyboxEnabled(Bool enabled)
	{
		skyboxEnabled = enabled;
	}

	Bool Camera::IsSkyboxEnabled() const
	{
		return skyboxEnabled;
	}

	SceneObjectRef Camera::GetSkyboxSceneObject()
	{
		if (sharedSkyboxCamera.IsValid())return sharedSkyboxCamera->GetSkyboxSceneObject();
		return skyboxSceneObject;
	}

	TextureRef Camera::GetSkyboxTexture()
	{
		if (sharedSkyboxCamera.IsValid())return sharedSkyboxCamera->GetSkyboxTexture();
		return skyboxTexture;
	}

	MaterialRef Camera::GetSkyboxMaterial()
	{
		if (sharedSkyboxCamera.IsValid())return sharedSkyboxCamera->GetSkyboxMaterial();
		return skyboxMaterial;
	}

	void Camera::SetSSAOEnabled(Bool enabled)
	{
		ssaoEnabled = enabled;
	}

	Bool Camera::IsSSAOEnabled()
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

	void Camera::SetRenderOrderIndex(UInt32 index)
	{
		renderOrderIndex = index;
	}

	UInt32 Camera::GetRenderOrderIndex()
	{
		return renderOrderIndex;
	}

	void Camera::AddClearBuffer(RenderBufferType buffer)
	{
		IntMaskUtil::SetBitForMask(&clearBufferMask, (UInt32)buffer);
	}

	void Camera::RemoveClearBuffer(RenderBufferType buffer)
	{
		IntMaskUtil::ClearBitForMask(&clearBufferMask, (UInt32)buffer);
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

	void Camera::SetupOffscreenRenderTarget(Int32 width, Int32 height)
	{
		SetupOffscreenRenderTarget(width, height, false);
	}

	void Camera::SetupOffscreenRenderTarget(Int32 width, Int32 height, Bool cube)
	{
		// get reference to the engine's object manager
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		if (renderTarget.IsValid() && renderTarget->GetObjectID() != Engine::Instance()->GetGraphicsSystem()->GetDefaultRenderTarget()->GetObjectID())
		{
			objectManager->DestroyRenderTarget(renderTarget);
		}

		TextureAttributes colorAttributes;
		colorAttributes.FilterMode = TextureFilter::Linear;
		colorAttributes.MipMapLevel = 4;
		colorAttributes.WrapMode = TextureWrap::Clamp;
		colorAttributes.IsCube = cube;
		renderTarget = objectManager->CreateRenderTarget(true, true, true, colorAttributes, width, height);

		UpdateDisplay();
	}

	RenderTargetRef Camera::GetRenderTarget()
	{
		if (!renderTarget.IsValid())return Engine::Instance()->GetGraphicsSystem()->GetDefaultRenderTarget();
		else return renderTarget;
	}

	void Camera::SetWidthHeightRatio(Real width, Real height)
	{
		this->widthHeightRatio = width / height;
		UpdateDisplay();
	}

	void Camera::SetupCopyRenderTarget()
	{
		// get reference to the engine's object manager
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		if (copyTarget.IsValid())
		{
			objectManager->DestroyRenderTarget(copyTarget);
		}

		TextureAttributes colorAttributes;
		colorAttributes.FilterMode = TextureFilter::Linear;
		colorAttributes.MipMapLevel = 4;
		colorAttributes.WrapMode = TextureWrap::Clamp;

		RenderTargetRef renderTarget = GetRenderTarget();
		copyTarget = objectManager->CreateRenderTarget(true, false, false, colorAttributes, renderTarget->GetWidth(), renderTarget->GetHeight());
	}

	RenderTargetRef Camera::GetCopyRenderTarget()
	{
		return copyTarget;
	}

	void Camera::UpdateDisplay()
	{
		Matrix4x4 proj;

		Real ratio = widthHeightRatio;

		if (projectionMode == ProjectionMode::Perspective)
		{
			Transform::BuildPerspectiveProjectionMatrix(proj, fov, ratio, 5, 200);
		}
		else
		{
			RenderTargetRef renderTarget = GetRenderTarget();
			Real height = (Real)renderTarget->GetHeight();
			Real width = (Real)renderTarget->GetWidth();
			Real halfWidth = width / 2.0f;
			Real halfHeight = height / 2.0f;
			Real left = -halfWidth;
			Real right = left + width;
			Real top = halfHeight;
			Real bottom = halfHeight - height;
			Real near = 5;
			Real far = 200;
			Transform::BuildOrthographicProjectionMatrix(proj, top, bottom, left, right, near, far);
		}
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

	void Camera::SetFOV(Real fov)
	{
		this->fov = fov;
		UpdateDisplay();
	}

	void Camera::SetSkyboxTextureTransform(Transform& trans)
	{
		skyboxTextureTransform = trans;
	}

	const Transform& Camera::GetSkyboxTransform()
	{
		return skyboxTextureTransform;
	}

	Bool Camera::AddClipPlane(const Vector3& normal, Real offset)
	{
		NONFATAL_ASSERT_RTRN(clipPlaneCount < Constants::MaxClipPlanes, "Camera::AddClipPlane -> Maximum clip planes exceeded.", false, true);
		clipPlanes[clipPlaneCount].Normal = normal;
		clipPlanes[clipPlaneCount].Offset = offset;
		clipPlaneCount++;
		return true;
	}

	const ClipPlane* Camera::GetClipPlane(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < clipPlaneCount, "Camera::GetClipPlane -> 'index' is out of range.", nullptr, true);
		return clipPlanes + index;
	}

	UInt32 Camera::GetClipPlaneCount() const
	{
		return clipPlaneCount;
	}

	void Camera::SetReverseCulling(Bool reverseCulling)
	{
		this->reverseCulling = reverseCulling;
	}

	Bool Camera::GetReverseCulling()
	{
		return reverseCulling;
	}

	void Camera::SetUniformWorldSceneObjectTransform(const Transform& transform)
	{
		uniformWorldSceneObjectTransform = transform;
	}

	const Transform& Camera::GetUniformWorldSceneObjectTransform()
	{
		return uniformWorldSceneObjectTransform;
	}

	void Camera::SetProjectionMode(ProjectionMode mode)
	{
		projectionMode = mode;
		UpdateDisplay();
	}
}

