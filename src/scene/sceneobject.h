#ifndef _GTE_SCENEOBJECT_H_
#define _GTE_SCENEOBJECT_H_

#include <vector>
#include <string>

#include "engine.h"
#include "geometry/sceneobjecttransform.h"
#include "object/engineobject.h"
#include "base/bitmask.h"

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

	class SceneObjectProcessingDescriptor
	{
		public:

		Transform AggregateTransform;
		Transform AggregateTransformInverse;
		Bool WillRenderCalled;

		SceneObjectProcessingDescriptor()
		{
			WillRenderCalled = false;
		}
	};

	class SceneObject : public EngineObject
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		// all render managers need access to the aggregate transform
		friend class ForwardRenderManager;

		// all scene managers need access to the aggregate transform
		friend class SceneManager;

		// SceneObjectSkeletonNode needs access to the aggregate transform
		friend class SceneObjectSkeletonNode;

		// Mesh3DFilter needs to be a friend so that it can update
		// any attached renderer when its mesh is updated
		friend class Mesh3DFilter;

	protected:

		SceneObjectProcessingDescriptor processingDescriptor;

		std::string name;
		Bool isActive;
		Bool isStatic;
		Bool inheritTransform;
		SceneObjectTransform transform;
		std::vector<SceneObjectSharedPtr> children;
		SceneObjectSharedPtr parent;
		IntMask layerMask;
		CameraSharedPtr camera;
		LightSharedPtr light;
		RendererSharedPtr renderer;
		Mesh3DFilterSharedPtr mesh3DFilter;
		ParticleSystemSharedPtr particleSystem;

		SceneObject();
		virtual ~SceneObject();

		SceneObjectProcessingDescriptor& GetProcessingDescriptor();
		void NotifyNewMesh3D();

	public:

		Bool IsActive();
		void SetActive(Bool active);
		Bool IsStatic();
		void SetStatic(Bool isStatic);
		Bool InheritsTransform() const;
		void SetInheritsTransform(Bool inherit);
		void SetName(const std::string& name);
		const std::string& GetName();

		void SetLayerMask(IntMask mask);
		IntMask GetLayerMask() const;

		SceneObjectTransform& GetTransform();
		const SceneObjectTransform& GetConstTransform() const;

		Bool SetRenderer(RendererRef renderer);
		Bool SetMesh3DFilter(Mesh3DFilterRef filter);
		Bool SetCamera(CameraRef camera);
		Bool SetLight(LightRef light);
		Bool SetParticleSystem(ParticleSystemRef particleSystem);

		Bool RemoveRenderer();
		Bool RemoveMesh3DFilter();
		Bool RemoveCamera();
		Bool RemoveLight();
		Bool RemoveParticleSystem();
	
		Mesh3DRef GetMesh3D();
		Mesh3DFilterRef GetMesh3DFilter();
		RendererRef GetRenderer();
		CameraRef GetCamera();
		LightRef GetLight();
		ParticleSystemRef GetParticleSystem();

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
