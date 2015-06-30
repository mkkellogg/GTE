#ifndef _GTE_SCENEOBJECT_H_
#define _GTE_SCENEOBJECT_H_

#include "geometry/sceneobjecttransform.h"
#include "engineobject.h"
#include "enginetypes.h"
#include "base/intmask.h"
#include <vector>
#include <memory>
#include <string>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;
	class SubMesh3DRenderer;
	class SubMesh3D;
	class Mesh3DRenderer;
	class SkinnedMesh3DRenderer;
	class Mesh3D;
	class Transform;
	class Camera;
	class SceneObject;
	class Light;

	class SceneObject : public EngineObject
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		friend class RenderManager;

		// Mesh3DFilter needs to be a friend so that it can update
		// any attached renderer when its mesh is updated
		friend class Mesh3DFilter;

	protected:

		std::string name;
		Bool isActive;
		Bool isStatic;
		SceneObjectTransform transform;
		Transform processingTransform;
		std::vector<SceneObjectRef > children;
		SceneObjectRef parent;
		IntMask layerMask;
		CameraRef camera;
		LightRef light;
		Mesh3DRendererRef renderer3D;
		SkinnedMesh3DRendererRef skinnedRenderer3D;
		Mesh3DFilterRef mesh3DFilter;

		SceneObject();
		virtual ~SceneObject();

		void SetAggregateTransform(Transform& transform);
		void NotifyNewMesh3D();

	public:

		Bool IsActive();
		void SetActive(Bool active);
		Bool IsStatic();
		void SetStatic(Bool isStatic);
		void SetName(const std::string& name);
		const std::string& GetName();

		void SetLayerMask(IntMask mask);
		IntMask GetLayerMask() const;

		SceneObjectTransform& GetTransform();
		const SceneObjectTransform& GetConstTransform() const;
		const Transform& GetAggregateTransform() const;

		Bool SetMesh3DRenderer(Mesh3DRendererRef renderer);
		Bool SetSkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer);
		Bool SetMesh3DFilter(Mesh3DFilterRef filter);

		Bool SetCamera(CameraRef camera);
		Bool SetLight(LightRef light);

		Mesh3DRef GetMesh3D();
		Mesh3DFilterRef GetMesh3DFilter();
		Mesh3DRendererRef GetMesh3DRenderer();
		SkinnedMesh3DRendererRef GetSkinnedMesh3DRenderer();

		CameraRef GetCamera();
		LightRef GetLight();

		void AddChild(SceneObjectRef child);
		void RemoveChild(SceneObjectRef child);
		UInt32 GetChildrenCount() const;
		SceneObjectRef GetChildAt(UInt32 index) const;
		SceneObjectRef GetParent() const;

		// TODO: optimize this hashing function (implement correctly)
		typedef struct
		{
			Int32 operator()(const SceneObject& s) const
			{
				return (Int32)s.GetObjectID() << 1;
			}
		}SceneObjectHasher;

		typedef struct
		{
			Bool operator() (const SceneObject& a, const SceneObject& b) const { return a == b; }
		} SceneObjectEq;

		Bool operator==(const SceneObject& s) const
		{
			return s.GetObjectID() == this->GetObjectID();
		}
	};
}

#endif
