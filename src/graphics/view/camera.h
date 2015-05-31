#ifndef _GTE_CAMERA_H_
#define _GTE_CAMERA_H_

#include "geometry/transform.h"
#include "geometry/vector/vector3.h"
#include "object/sceneobjectcomponent.h"
#include "graphics/render/rendertarget.h"
#include "graphics/graphics.h"
#include "object/enginetypes.h"
#include "base/intmask.h"
#include "global/constants.h"

namespace GTE
{
	//forward declarations
	class Camera;
	class Graphics;

	class ClipPlane
	{
	public:

		Vector3 Normal;
		Real Offset;
	};

	enum class ProjectionMode
	{
		Perspective = 0,
		Orthographic = 1
	};

	class Camera : public SceneObjectComponent
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		UInt32 clearBufferMask;

		Transform projection;
		Transform skyboxTextureTransform;

		bool skyboxSetup;
		bool skyboxEnabled;
		MaterialRef skyboxMaterial;
		TextureRef skyboxTexture;
		SceneObjectRef skyboxSceneObject;
		Mesh3DRef skyboxMesh;
		Mesh3DFilterRef skyboxMeshFilter;
		Mesh3DRendererRef skyboxMeshRenderer;
		CameraRef sharedSkyboxCamera;

		SSAORenderMode ssaoMode;
		bool ssaoEnabled;

		UInt32 renderOrderIndex;

		RenderTargetRef renderTarget;

		RenderTargetRef copyTarget;

		IntMask cullingMask;

		Real fov;
		Real widthHeightRatio;

		UInt32 clipPlaneCount;
		ClipPlane clipPlanes[Constants::MaxClipPlanes];

		Transform uniformWorldSceneObjectTransform;
		bool reverseCulling;

		ProjectionMode projectionMode;

		Camera();
		~Camera();

	public:

		void SetupSkybox(TextureRef cubeTexture);
		void ShareSkybox(CameraRef camera);
		bool IsSkyboxSetup() const;
		void SetSkyboxEnabled(bool enabled);
		bool IsSkyboxEnabled() const;
		SceneObjectRef GetSkyboxSceneObject();
		TextureRef GetSkyboxTexture();
		MaterialRef GetSkyboxMaterial();

		void SetSSAOEnabled(bool enabled);
		bool IsSSAOEnabled();
		void SetSSAORenderMode(SSAORenderMode mode);
		SSAORenderMode GetSSAORenderMode();

		void SetRenderOrderIndex(UInt32 index);
		UInt32 GetRenderOrderIndex();

		const Transform& GetProjectionTransform() const;
		void TransformProjectionTransformBy(const Transform& transform);
		void PreTransformProjectionTransformBy(const Transform& transform);

		void AddClearBuffer(RenderBufferType buffer);
		void RemoveClearBuffer(RenderBufferType buffer);
		IntMask GetClearBufferMask() const;

		void SetupOffscreenRenderTarget(int width, int height);
		void SetupOffscreenRenderTarget(int width, int height, bool cube);
		RenderTargetRef GetRenderTarget();
		void SetWidthHeightRatio(Real width, Real height);

		void SetupCopyRenderTarget();
		RenderTargetRef GetCopyRenderTarget();

		void UpdateDisplay();

		void SetCullingMask(IntMask mask);
		void MergeCullingMask(IntMask mask);
		IntMask GetCullingMask() const;

		void SetFOV(Real fov);

		void SetSkyboxTextureTransform(Transform& trans);
		const Transform& GetSkyboxTransform();

		bool AddClipPlane(const Vector3& normal, Real offset);
		const ClipPlane* GetClipPlane(UInt32 index);
		UInt32 GetClipPlaneCount() const;

		void SetReverseCulling(bool reverseCulling);
		bool GetReverseCulling();

		void SetUniformWorldSceneObjectTransform(const Transform& transform);
		const Transform& GetUniformWorldSceneObjectTransform();

		void SetProjectionMode(ProjectionMode mode);
	};
}

#endif
