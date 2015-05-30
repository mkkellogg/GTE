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
		bool isActive;
		bool isStatic;
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

		bool IsActive();
		void SetActive(bool active);
		bool IsStatic();
		void SetStatic(bool isStatic);
		void SetName(const std::string& name);
		const std::string& GetName();

		void SetLayerMask(IntMask mask);
		IntMask GetLayerMask() const;

		SceneObjectTransform& GetTransform();
		const Transform& GetAggregateTransform() const;

		bool SetMesh3DRenderer(Mesh3DRendererRef renderer);
		bool SetSkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer);
		bool SetMesh3DFilter(Mesh3DFilterRef filter);

		bool SetCamera(CameraRef camera);
		bool SetLight(LightRef light);

		Mesh3DRef GetMesh3D();
		Mesh3DFilterRef GetMesh3DFilter();
		Mesh3DRendererRef GetMesh3DRenderer();
		SkinnedMesh3DRendererRef GetSkinnedMesh3DRenderer();

		CameraRef GetCamera();
		LightRef GetLight();

		void AddChild(SceneObjectRef child);
		void RemoveChild(SceneObjectRef child);
		unsigned int GetChildrenCount() const;
		SceneObjectRef GetChildAt(unsigned int index) const;
		SceneObjectRef GetParent();

		// TODO: optimize this hashing function (implement correctly)
		typedef struct
		{
			int operator()(const SceneObject& s) const
			{
				return (int)s.GetObjectID() << 1;
			}
		}SceneObjectHasher;

		typedef struct
		{
			bool operator() (const SceneObject& a, const SceneObject& b) const { return a == b; }
		} SceneObjectEq;

		bool operator==(const SceneObject& s) const
		{
			return s.GetObjectID() == this->GetObjectID();
		}
	};
}

#endif
