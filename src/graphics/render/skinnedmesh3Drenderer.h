#ifndef _SKINNED_MESH3D_RENDERER_H_
#define _SKINNED_MESH3D_RENDERER_H_

// forward declarations

#include "mesh3Drenderer.h"
#include "skinnedmesh3Dattrtransformer.h"

class SkinnedMesh3DRenderer : public Mesh3DRenderer
{
	friend class EngineObjectManager;
	friend class SubMesh3DRenderer;

	protected:

	SkinnedMesh3DAttributeTransformer meshTransformer;
	Mesh3DRef mesh;
	SkinnedMesh3DRenderer();
	~SkinnedMesh3DRenderer();

	public:

	Mesh3DRef GetMesh();
	SubMesh3DRef GetSubMesh(unsigned int index);
	void SetMesh(Mesh3DRef mesh);
	void UpdateFromMesh();
};


#endif
