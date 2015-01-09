#ifndef _GTE_SKINNED_MESH3D_RENDERER_H_
#define _GTE_SKINNED_MESH3D_RENDERER_H_

// forward declarations
class Skeleton;
class VertexBoneMap;

#include "mesh3Drenderer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include <unordered_map>

class SkinnedMesh3DRenderer : public Mesh3DRenderer
{
	friend class EngineObjectManager;
	friend class SubMesh3DRenderer;

	protected:

	SkeletonRef skeleton;

	// all the VertexBoneMap objects for this renderer
	std::vector<VertexBoneMap *> vertexBoneMaps;

	SkinnedMesh3DAttributeTransformer meshTransformer;
	std::unordered_map<unsigned int, unsigned int>subMeshIndexMap;

	SkinnedMesh3DRenderer();
	~SkinnedMesh3DRenderer();

	public:

	void SetSkeleton(SkeletonRef skeleton);
	SkeletonRef GetSkeleton();
	void UpdateFromMesh();
	void MapSubMeshToVertexBoneMap(unsigned int subMeshIndex, unsigned int vertexBoneMapIndex);

    void AddVertexBoneMap(VertexBoneMap * map);
    VertexBoneMap * GetVertexBoneMap(unsigned int index);
};


#endif
