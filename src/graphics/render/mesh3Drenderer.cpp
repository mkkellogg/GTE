#include "mesh3Drenderer.h"
#include "submesh3Drenderer.h"
#include "multimaterial.h"
#include "object/engineobjectmanager.h"
#include "scene/sceneobjectcomponent.h"
#include "scene/sceneobject.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE {
    /*
    * Default constructor
    */
    Mesh3DRenderer::Mesh3DRenderer() {

    }

    /*
     * Clean-up
     */
    Mesh3DRenderer::~Mesh3DRenderer() {

    }

    /*
     * Deallocate and destroy each sub-renderer contained by this instance.
     */
    void Mesh3DRenderer::DestroyRenderers() {
        for (UInt32 i = 0; i < subRenderers.size(); i++) {
            DestroyRenderer(i);
        }
        subRenderers.clear();
    }

    /*
     * Deallocate and destroy the sub-renderer at [index] in the member list of
     * sub-renderers: [subRenderers].
     */
    void Mesh3DRenderer::DestroyRenderer(UInt32 index) {
        EngineObjectManager *objectManager = Engine::Instance()->GetEngineObjectManager();
        if (index < subRenderers.size()) {
            SubMesh3DRendererRef renderer = subRenderers[index];
            if (renderer.IsValid()) {
                objectManager->DestroySubMesh3DRenderer(renderer);
            }
            subRenderers.erase(subRenderers.begin() + index);
        }
    }

    /*
     * Get the number of materials attached to this instance.
     */
    UInt32 Mesh3DRenderer::GetMultiMaterialCount() const {
        return (UInt32)materials.size();
    }

    /*
     * Get a reference to the material at [index] in the member list of materials, [materials].
     */
    MultiMaterialRef Mesh3DRenderer::GetMultiMaterial(UInt32 index) {
        NONFATAL_ASSERT_RTRN(index < GetMultiMaterialCount(), "Mesh3DRenderer::GetMaterial -> 'index' is out of range.", NullMultiMaterialRef, true);
        return materials[index];
    }

    /*
     * Set the material at [index] in the member list of materials, [materials].
     */
    void Mesh3DRenderer::SetMultiMaterial(UInt32 index, MultiMaterialRef material) {
        NONFATAL_ASSERT(material.IsValid(), "Mesh3DRenderer::SetMaterial -> 'material' is null.", true);
        NONFATAL_ASSERT(index < GetMultiMaterialCount(), "Mesh3DRenderer::SetMaterial -> 'index' is out of range.", true);

        materials[index] = material;
    }

    /*
    * Create a multi-material to hold [material] and then add to [materials].
    */
    void Mesh3DRenderer::AddMultiMaterial(MaterialRef material) {
        EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

        MultiMaterialRef multiMaterial = objectManager->CreateMultiMaterial();
        NONFATAL_ASSERT(multiMaterial.IsValid(), "Mesh3DRenderer::AddMaterial -> Unable to create MultiMaterial to hold 'material'.", true);

        multiMaterial->AddMaterial(material);

        AddMultiMaterial(multiMaterial);
    }

    /*
     * Add a material to the member list of materials, [materials].
     */
    void Mesh3DRenderer::AddMultiMaterial(MultiMaterialRef material) {
        NONFATAL_ASSERT(material.IsValid(), "Mesh3DRenderer::AddMaterial -> 'material' is null.", true);
        materials.push_back(material);
    }

    /*
     * This method should be triggered every time the mesh for which this renderer is responsible
     * gets set.
     *
     * GetTargetMesh() returns the mesh in question. Typically it will be the Mesh3D object that is
     * attached to the same SceneObject as this renderer. If the target mesh is valid, it calls UpdateFromMesh()
     * specifically for that mesh.
     */
    void Mesh3DRenderer::InitializeForMesh() {
        NONFATAL_ASSERT(sceneObject.IsValid(), "Mesh3DRenderer::UpdateFromMesh -> 'sceneObject' is null.", true);

        Mesh3DRef mesh = GetTargetMesh();
        NONFATAL_ASSERT(mesh.IsValid(), "Mesh3DRenderer::UpdateFromMesh -> mesh is null.", true);

        InitializeForMesh(mesh);
    }

    /*
     * Update this renderer and prepare it for rendering of the sub-meshes contained in [mesh].
     */
    void Mesh3DRenderer::InitializeForMesh(Mesh3DConstRef mesh) {
        EngineObjectManager * engineObjectManager = Engine::Instance()->GetEngineObjectManager();
        UInt32 subMeshCount = mesh->GetSubMeshCount();

        // if the number of sub-renderers currently contained by this instance is larger than
        // the number of sub-meshes contained by [mesh], then delete the excess sub-renderers
        if (subMeshCount < subRenderers.size()) {
            for (UInt32 i = (UInt32)subRenderers.size(); i > subMeshCount; i--) {
                DestroyRenderer(i - 1);
            }
        }
        // if the number of sub-renderers currently contained by this instance is less than
        // the number of sub-meshes contained by [mesh], create enough new sub-renderers to make those
        // numbers match
        else if (subMeshCount > subRenderers.size()) {
            for (UInt32 i = (UInt32)subRenderers.size(); i < subMeshCount; i++) {
                SubMesh3DRendererRef renderer = engineObjectManager->CreateSubMesh3DRenderer();
                NONFATAL_ASSERT(renderer.IsValid(), "Mesh3DRenderer::UpdateFromMesh(Mesh3DSharedPtr) -> Could not create new SubMesh3DRenderer.", false);

                renderer->SetTargetSubMeshIndex(i);
                renderer->SetContainerRenderer(this);
                subRenderers.push_back(renderer);
            }
        }

        // for each sub-renderer, call UpdateFromSubMesh() to update the sub-renderer
        // for its corresponding sub-mesh in [mesh]
        for (UInt32 i = 0; i < subMeshCount; i++) {
            UpdateFromSubMesh(i);
        }
    }

    /*
     * Update the sub-render at [index] in the member list of sub-renderers [subRenderers]
     * for the sub-mesh at [index] in the target mesh for this renderer.
     */
    void Mesh3DRenderer::UpdateFromSubMesh(UInt32 index) {
        NONFATAL_ASSERT(index < subRenderers.size(), "Mesh3DRenderer::UpdateFromSubMesh -> 'index' is out of range.", true);

        SubMesh3DRendererRef renderer = subRenderers[index];
        renderer->UpdateFromMesh();
    }

    /*
     * Get the target mesh for this renderer. The Mesh3D object that is attached to the same SceneObject
     * as this renderer will implicitly be the target mesh.
     */
    Mesh3DRef Mesh3DRenderer::GetTargetMesh() {
        NONFATAL_ASSERT_RTRN(sceneObject.IsValid(), "Mesh3DRenderer::GetTargetMesh -> 'sceneObject' is null.", NullMesh3DRef, true);

        Mesh3DRef mesh = sceneObject->GetMesh3D();

        return mesh;
    }

    /*
     * Get the sub-mesh contained in the target mesh for this renderer that corresponds to [subRenderer]. This will be
     * the SubMesh3D instance that [subRenderer] is responsible for rendering.
     */
    SubMesh3DRef Mesh3DRenderer::GetSubMeshForSubRenderer(SubMesh3DRendererConstRef subRenderer) {
        NONFATAL_ASSERT_RTRN(subRenderer.IsValid(), "Mesh3DRenderer::GetSubMeshForSubRenderer -> 'subRenderer' is null.", NullSubMesh3DRef, true);
        NONFATAL_ASSERT_RTRN(sceneObject.IsValid(), "Mesh3DRenderer::GetSubMeshForSubRenderer -> 'sceneObject' is null.", NullSubMesh3DRef, true);

        // this loop finds the index in [subRenderers] to which [subRenderer] belongs
        for (UInt32 i = 0; i < subRenderers.size(); i++) {
            if (subRenderers[i] == subRenderer) {
                Mesh3DRef mesh = GetTargetMesh();
                NONFATAL_ASSERT_RTRN(mesh.IsValid(), "Mesh3DRenderer::GetSubMeshForSubRenderer -> 'mesh' is null.", NullSubMesh3DRef, false);

                SubMesh3DRef subMesh = mesh->GetSubMesh(i);
                NONFATAL_ASSERT_RTRN(subMesh.IsValid(), "Mesh3DRenderer::GetSubMeshForSubRenderer -> 'subMesh' is null.", NullSubMesh3DRef, false);

                return subMesh;
            }
        }

        return NullSubMesh3DRef;
    }

    /*
     * Get the sub-mesh at [index] in the target mesh for this renderer.
     */
    SubMesh3DRef Mesh3DRenderer::GetSubMesh(UInt32 index) {
        NONFATAL_ASSERT_RTRN(sceneObject.IsValid(), "Mesh3DRenderer::GetSubMesh -> 'sceneObject' is null.", NullSubMesh3DRef, true);

        Mesh3DRef mesh = GetTargetMesh();
        NONFATAL_ASSERT_RTRN(mesh.IsValid(), "Mesh3DRenderer::GetSubMesh -> 'mesh' is null.", NullSubMesh3DRef, true);

        SubMesh3DRef subMesh = mesh->GetSubMesh(index);
        NONFATAL_ASSERT_RTRN(subMesh.IsValid(), "Mesh3DRenderer::GetSubMesh -> 'subMesh' is null.", NullSubMesh3DRef, true);

        return subMesh;
    }

    /*
     * Get the sub-renderer at [index] in the member list of sub-renderers, [subRenderers].
     */
    SubMesh3DRendererRef Mesh3DRenderer::GetSubRenderer(UInt32 index) {
        if (index >= subRenderers.size()) {
            Debug::PrintError("Mesh3DRenderer::GetSubRenderer -> 'index' is out of range.");
            return NullSubMesh3DRendererRef;
        }

        return subRenderers[index];
    }

    /*
     * Get the number of sub-renderers managed by this renderer.
     */
    UInt32 Mesh3DRenderer::GetSubRendererCount() const {
        return (UInt32)subRenderers.size();
    }
}
