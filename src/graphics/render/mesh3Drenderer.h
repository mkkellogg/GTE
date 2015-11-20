/*
 * class: Mesh3DRenderer
 *
 * author: Mark Kellogg
 *
 * Mesh3DRenderer is a container for SubMesh3DRenderer, which performs
 * all the work a of rendering a single mesh/vertex group. This class
 * complements Mesh3D, which is a container class for SubMesh3D. SubMesh3D
 * contains all the position, UV, color, and normal data for a single
 * mesh, and Mesh3D is a convenience class for grouping those meshes. Each
 * instance of SubMesh3DRenderer contained in Mesh3DRenderer is responsible
 * for rendering exactly one of those sub-meshes.
 *
 * Typically a Mesh3D object will be attached to the same SceneObject as a
 * Mesh3DRenderer object. That Mesh3D object implicitly becomes the target mesh
 * for the renderer.
 */

#ifndef _GTE_MESH3D_RENDERER_H_
#define _GTE_MESH3D_RENDERER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "scene/sceneobjectcomponent.h"
#include "renderer.h"

#include <vector>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;
	class SubMesh3DRenderer;
	class SubMesh3D;
	class Material;
	class Mesh3D;

	class Mesh3DRenderer : public Renderer
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		friend class ForwardRenderManager;

	protected:

		// list of materials that will be used to render the sub-meshes
		std::vector <MaterialSharedPtr> materials;
		// the renderers for each of the sub-meshes
		std::vector <SubMesh3DRendererSharedPtr> subRenderers;

		Mesh3DRenderer();
		virtual ~Mesh3DRenderer();
		void DestroyRenderers();
		void DestroyRenderer(UInt32 index);

	public:

		UInt32 GetMaterialCount() const;
		MaterialRef GetMaterial(UInt32 index);
		void SetMaterial(UInt32 index, MaterialRef material);
		void AddMaterial(MaterialRef material);

		virtual void InitializeForMesh();
		void InitializeForMesh(Mesh3DConstRef mesh);
		void UpdateFromSubMesh(UInt32 index);

		virtual Mesh3DRef GetTargetMesh();
		SubMesh3DRef GetSubMeshForSubRenderer(SubMesh3DRendererConstRef subRenderer);
		virtual SubMesh3DRef GetSubMesh(UInt32 index);
		SubMesh3DRendererRef GetSubRenderer(UInt32 index);
		UInt32 GetSubRendererCount() const;
	};
}

#endif
