#ifndef _SKINNED_MESH3D_RENDERER_H_
#define _SKINNED_MESH3D_RENDERER_H_

// forward declarations

#include "mesh3Drenderer.h"
#include "skinnedmeshattrtransformer.h"

class SkinnedMesh3DRenderer : public Mesh3DRenderer
{
	protected:

	SkinnedMeshAttributeTransformer meshTransformer;
	Mesh3DRef mesh;
	SkinnedMesh3DRenderer();
	~SkinnedMesh3DRenderer();

	public:

	void SetMesh(Mesh3DRef mesh);
	void UpdateFromMesh();
};


#endif
