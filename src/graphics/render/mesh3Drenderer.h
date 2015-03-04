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

//forward declarations
class EngineObjectManager;
class SubMesh3DRenderer;
class SubMesh3D;
class Material;
class Mesh3D;

#include "object/engineobject.h"
#include "object/sceneobjectcomponent.h"
#include <vector>

class Mesh3DRenderer : public SceneObjectComponent
{
	// Since this ultimately derives from EngineObject, we make this class
	// a friend of EngineObjectManager, and the constructor & destructor
	// protected so its life-cycle can be handled completely by EngineObjectManager.
	friend class EngineObjectManager;

	protected :

	// list of materials that will be used to render the sub-meshes
	std::vector <MaterialRef> materials;
	// the renderers for each of the sub-meshes
	std::vector <SubMesh3DRendererRef> subRenderers;

	Mesh3DRenderer();
    virtual ~Mesh3DRenderer();
    void DestroyRenderers();
    void DestroyRenderer(unsigned int index);

	public:

    unsigned int GetMaterialCount() const;
    MaterialRef GetMaterial(unsigned int index);
    void SetMaterial(unsigned int index, MaterialRef material);
    void AddMaterial(MaterialRef material);

    virtual void InitializeForMesh();
    void InitializeForMesh(Mesh3DRef mesh);
    void UpdateFromSubMesh(unsigned int index);

    virtual Mesh3DRef GetTargetMesh();
    SubMesh3DRef GetSubMeshForSubRenderer(SubMesh3DRendererRef subRenderer);
    virtual SubMesh3DRef GetSubMesh(unsigned int index);
    SubMesh3DRendererRef GetSubRenderer(unsigned int index);
    unsigned int GetSubRendererCount() const;
};

#endif
