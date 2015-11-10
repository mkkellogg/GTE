/*
 * class: Mesh3D
 *
 * author: Mark Kellogg
 *
 * Mesh3D is a container for multiple SubMesh3D objects. SubMesh3D contains
 * the actual vertex attribute data (vertex positions, vertex normals, UV
 * coordinates, etc...) for a mesh, and Mesh3D acts to organize them into
 * a single object.
 */

#ifndef _GTE_MESH3D_H_
#define _GTE_MESH3D_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/light/light.h"
#include <vector>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;
	class SubMesh3D;

	class Mesh3D : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		friend class SubMesh3D;

	protected:

		// numnber of sub-meshes managed by the Mesh3D instance
		UInt32 subMeshCount;
		// container for the submeshes
		std::vector<SubMesh3DSharedPtr> subMeshes;

		Mesh3D(UInt32 subMeshCount);
		~Mesh3D();
		void Destroy();

	public:

		UInt32 GetSubMeshCount() const;
		void SetSubMesh(SubMesh3DRef mesh, UInt32 index);
		SubMesh3DRef GetSubMesh(UInt32 index);
		Bool Init();
		void Update();

	};
}

#endif

