/*
 * class: ParticleMeshRenderer
 *
 * author: Mark Kellogg
 *
 * This class is responsible for rendering a particle system's mesh. It
 * mostly makes use of the functionality inheirted from Mesh3DRenderer.
 */

#ifndef _GTE_PARTICLE_MESH_RENDERER_H_
#define _GTE_PARTICLE_MESH_RENDERER_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "object/sceneobjectcomponent.h"
#include "graphics/render/mesh3Drenderer.h"
#include <vector>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;

	class ParticleMeshRenderer : public Mesh3DRenderer
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		// needed since ForwardRenderManager may need to directly access this renderer
		friend class ForwardRenderManager;
		// Particle system needs direct access to thsi class
		friend class ParticleSystem;

	protected:
		
		SceneObjectSharedPtr particleSystemObject;

		ParticleMeshRenderer();
		virtual ~ParticleMeshRenderer();

		void SetTargetSystemObject(SceneObjectRef particleSystemObject);
		void WillRender() override;

	public:

	};
}

#endif
