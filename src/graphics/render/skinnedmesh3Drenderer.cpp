#include "skinnedmesh3Drenderer.h"
#include "submesh3Drenderer.h"
#include "object/engineobjectmanager.h"
#include "scene/sceneobjectcomponent.h"
#include "scene/sceneobject.h"
#include "graphics/animation/vertexbonemap.h"
#include "graphics/stdattributes.h"
#include "graphics/animation/skeleton.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE {
    /*
    * Default constructor.
    */
    SkinnedMesh3DRenderer::SkinnedMesh3DRenderer() {

    }

    /*
     * Clean-up.
     */
    SkinnedMesh3DRenderer::~SkinnedMesh3DRenderer() {
        // loop through each VertexBoneMap and delete
        for (UInt32 i = 0; i < vertexBoneMaps.size(); i++) {
            VertexBoneMap * map = vertexBoneMaps[i];
            SAFE_DELETE(map);
            vertexBoneMaps[i] = nullptr;
        }
        vertexBoneMaps.clear();
    }

    /*
     * Set the skeleton for this renderer.
     */
    void SkinnedMesh3DRenderer::SetSkeleton(SkeletonRef skeleton) {
        this->skeleton = skeleton;
    }

    /*
     * Get a reference to this renderer's skeleton.
     */
    SkeletonRef SkinnedMesh3DRenderer::GetSkeleton() {
        return skeleton;
    }

    /*
     * @Override Mesh3DRenderer::UpdateFromMesh()
     *
     * This method first calls the overridden parent method. It extends the functionality of that method by
     * initializing an instance of SkinnedMesh3DAttributeTransformer for each sub-mesh in the target mesh,
     * to perform vertex skinning for each.
     */
    void SkinnedMesh3DRenderer::InitializeForMesh() {
        // get the target mesh
        Mesh3DRef mesh = GetTargetMesh();
        //Mesh3DRef mesh = sceneObject->GetMesh3D();

        if (mesh.IsValid()) {
            // call the base method
            Mesh3DRenderer::InitializeForMesh(mesh);

            // loop through each sub-mesh in the target mesh
            for (UInt32 i = 0; i < mesh->GetSubMeshCount(); i++) {
                Int32 vertexBoneMapIndex = -1;
                SubMesh3DRendererRef subRenderer = this->GetSubRenderer(i);

                // does the sub-mesh at index [i] have a corresponding VertexBoneMap object?
                if (subMeshIndexMap.find(i) != subMeshIndexMap.end() && subMeshIndexMap[i] >= 0) {
                    vertexBoneMapIndex = subMeshIndexMap[i];

                    // only create  SkinnedMesh3DAttributeTransformer for the sub-mesh at [i]
                    // if has not already been done
                    if (subRenderer->GetAttributeTransformer() == nullptr) {
                        // setup the attribute transformer to transform normals, positions, and tangents
                        StandardAttributeSet attributes = StandardAttributes::CreateAttributeSet();
                        StandardAttributes::AddAttribute(&attributes, StandardAttribute::Position);
                        StandardAttributes::AddAttribute(&attributes, StandardAttribute::Normal);
                        StandardAttributes::AddAttribute(&attributes, StandardAttribute::Tangent);

                        SkinnedMesh3DAttributeTransformer *attributeTransformer = new(std::nothrow) SkinnedMesh3DAttributeTransformer(attributes);
                        ASSERT(attributeTransformer != nullptr, "SkinnedMesh3DRenderer::UpdateFromMesh -> Could not allocate attribute transformer.");

                        subRenderer->SetAttributeTransformer(attributeTransformer);
                    }
                }
                else // since no vertex bone map exists for this sub-mesh, set the attribute transformer to null
                {
                    subRenderer->SetAttributeTransformer(nullptr);
                }

                // update the attribute transformer
                if (subRenderer->GetAttributeTransformer() != nullptr) {
                    SkinnedMesh3DAttributeTransformer * attrTransformer = dynamic_cast<SkinnedMesh3DAttributeTransformer*>(subRenderer->GetAttributeTransformer());
                    if (attrTransformer != nullptr) {
                        attrTransformer->SetRenderer(this);
                        attrTransformer->SetVertexBoneMapIndex(vertexBoneMapIndex);
                    }
                }
            }
        }
    }

    /*
     * Add a VertexBoneMap object to this renderer.
     */
    void SkinnedMesh3DRenderer::AddVertexBoneMap(VertexBoneMap * map) {
        vertexBoneMaps.push_back(map);
    }

    /*
     * Retrieve the VertexBoneMap object stored at [index] in [vertexBoneMaps].
     */
    VertexBoneMap * SkinnedMesh3DRenderer::GetVertexBoneMap(UInt32 index) {
        NONFATAL_ASSERT_RTRN(index < vertexBoneMaps.size(), "Mesh3D::GetVertexBoneMap -> 'index' is out of range.", nullptr, true);

        return vertexBoneMaps[index];
    }

    /*
     * Map the sub-mesh at [subMeshIndex] in the target mesh of this renderer to the VertexBoneMap structure at
     * [vertexBoneMapIndex] in the member list of VertexBoneMap objects: [vertexBoneMaps]. This establishes the link
     * between a sub-mesh (and therefore sub-renderer) and the corresponding VertexBoneMap instance.
     *
     * Setting vertexBoneMapIndex to -1 effectively turns off skinning for the specified sub-mesh
     */
    void SkinnedMesh3DRenderer::MapSubMeshToVertexBoneMap(UInt32 subMeshIndex, Int32 vertexBoneMapIndex) {
        subMeshIndexMap[subMeshIndex] = vertexBoneMapIndex;
    }
}
