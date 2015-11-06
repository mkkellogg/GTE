/*
 * class: Mesh3DRenderer
 *
 * author: Mark Kellogg
 *
 * SkinnedMesh3DRenderer extends Mesh3DRenderer by adding vertex skinning functionality.
 * SubMesh3DRenderer already has the concept of an attribute transformer built in, that is
 * an instance of AttributeTransformer or a deriving class can be supplied to it and it
 * will use that instance to transform the various attributes of its target mesh before it
 * does its rendering. SkinnedMesh3DRenderer supplies a special kind of AttributeTransformer
 * that performs vertex skinning.
 *
 */


#ifndef _GTE_SKINNED_MESH3D_RENDERER_H_
#define _GTE_SKINNED_MESH3D_RENDERER_H_

#include "mesh3Drenderer.h"
#include "skinnedmesh3Dattrtransformer.h"
#include <unordered_map>

namespace GTE
{
	// forward declarations
	class Skeleton;
	class VertexBoneMap;

	class SkinnedMesh3DRenderer : public Mesh3DRenderer
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		friend class SubMesh3DRenderer;

		// the skeleton for this skinned mesh renderer, describes the bone hierarchy
		// and (inverse) bind pose transformation of each bone. it also describes the scene
		// object to which each bone corresponds indirectly through the node structures
		// it contains.
		SkeletonSharedPtr skeleton;

		// all the VertexBoneMap objects for this renderer
		std::vector<VertexBoneMap *> vertexBoneMaps;

		// the attribute transformer for this renderer, does performs the vertex skinning
		SkinnedMesh3DAttributeTransformer attributeTransformer;

		// maps each sub-mesh in the target mesh of this renderer to its corresponding
		// VertexBoneMap structure, which describe the attachment of mesh vertices to
		// the bones in [skeleton]
		std::unordered_map<UInt32, int>subMeshIndexMap;

		SkinnedMesh3DRenderer();
		~SkinnedMesh3DRenderer();

	public:

		void SetSkeleton(SkeletonRef skeleton);
		SkeletonRef GetSkeleton();
		void InitializeForMesh();
		void MapSubMeshToVertexBoneMap(UInt32 subMeshIndex, Int32 vertexBoneMapIndex);

		void AddVertexBoneMap(VertexBoneMap * map);
		VertexBoneMap * GetVertexBoneMap(UInt32 index);
	};
}

#endif
