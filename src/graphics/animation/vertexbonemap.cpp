#include <memory.h>

#include "vertexbonemap.h"
#include "skeleton.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"

namespace GTE {
    /*
    * Only constructor, parameterized.
    */
    VertexBoneMap::VertexBoneMap(UInt32 vertexCount, UInt32 uVertexCount) {
        this->vertexCount = vertexCount;
        this->uniqueVertexCount = uVertexCount;
        mappingDescriptors = nullptr;
    }

    /*
     * Destructor.
     */
    VertexBoneMap::~VertexBoneMap() {
        Destroy();
    }

    /*
     * Destroy this VertexBoneMap object.
     */
    void VertexBoneMap::Destroy() {
        // delete [mappingDescriptors] and set to nullptr
        if (mappingDescriptors != nullptr) {
            delete[] mappingDescriptors;
            mappingDescriptors = nullptr;
        }
    }

    /*
     * Initialize this VertexBoneMap object. This method MUST be called before this object can be used.
     */
    Bool VertexBoneMap::Init() {
        // destroy existing data, if it exists
        Destroy();

        mappingDescriptors = new(std::nothrow) VertexMappingDescriptor[vertexCount];
        ASSERT(mappingDescriptors != nullptr, "VertexBoneMap::Init -> unable to allocate vertex mapping descriptors master array.");

        return true;
    }

    /*
     * Get VertexMappingDescriptor for vertex (non-unique) at [index]
     */
    VertexBoneMap::VertexMappingDescriptor* VertexBoneMap::GetDescriptor(UInt32 index) {
        if (index >= vertexCount) {
            Debug::PrintError("VertexBoneMap::GetDescriptor -> Index out of range.");
            return nullptr;
        }

        return mappingDescriptors + index;
    }

    /*
     * Get total number of vertices that are mapped.
     */
    UInt32 VertexBoneMap::GetVertexCount() const {
        return vertexCount;
    }

    /*
     * Get number of unique vertices that are mapped.
     */
    UInt32 VertexBoneMap::GetUniqueVertexCount() const {
        return uniqueVertexCount;
    }

    /*
     * Update the bone indices in this map to match that of [skeleton]
     */
    void VertexBoneMap::BindTo(SkeletonConstRef skeleton) {
        NONFATAL_ASSERT(skeleton.IsValid(), "VertexBoneMap::BindTo -> 'skeleton' is not valid.", true);

        for (UInt32 v = 0; v < vertexCount; v++) {
            VertexBoneMap::VertexMappingDescriptor * desc = GetDescriptor(v);
            for (UInt32 b = 0; b < desc->BoneCount; b++) {
                Int32 boneIndex = skeleton->GetBoneMapping(desc->Name[b]);
                desc->BoneIndex[b] = boneIndex;
            }
        }
    }

    /*
     * Create a full (deep) clone of this VertexBoneMap object.
     */
    VertexBoneMap * VertexBoneMap::FullClone() {
        // allocate new VertexBoneMap objects
        VertexBoneMap * clone = new(std::nothrow) VertexBoneMap(vertexCount, uniqueVertexCount);
        ASSERT(clone != nullptr, "VertexBoneMap::FullClone -> Could not allocate vertex bone map.");

        // initialize the new map
        Bool initSuccess = clone->Init();
        if (!initSuccess) {
            Debug::PrintError("VertexBoneMap::FullClone -> Could not initialize vertex bone map.");
            delete clone;
            return nullptr;
        }

        // copy over VertexMappingDescriptors one-by-one
        for (UInt32 v = 0; v < vertexCount; v++) {
            clone->GetDescriptor(v)->SetTo(GetDescriptor(v));
        }

        return clone;
    }
}
