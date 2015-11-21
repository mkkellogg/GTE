#include "engine.h"
#include "scene/sceneobjectcomponent.h"
#include "scene/sceneobject.h"
#include "object/engineobject.h"
#include "object/engineobjectmanager.h"
#include "mesh3D.h"
#include "submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	* Single constructor, only parameter [subMeshCount] is the number of
	* sub-meshes that will be managed by this Mesh3D instance.
	*/
	Mesh3D::Mesh3D(UInt32 subMeshCount)
	{
		if(subMeshCount <= 0)subMeshCount = 1;
		this->subMeshCount = subMeshCount;
	}

	/*
	 * Clean-up.
	 */
	Mesh3D::~Mesh3D()
	{
		Destroy();
	}

	/*
	 * Deallocate and destroy all objects created by this instance.
	 */
	void Mesh3D::Destroy()
	{
		subMeshes.clear();
	}

	/*
	 * Get the number of sub-meshes managed by this instance of Mesh3D.
	 */
	UInt32 Mesh3D::GetSubMeshCount() const
	{
		return subMeshCount;
	}

	/*
	 * Initialize this instance. This method will pre-allocate [subMeshCount] entries
	 * in [subMeshes], to be set at a later time.
	 */
	Bool Mesh3D::Init()
	{
		// if this instance has already been initialized, we must first destroy this instance.
		Destroy();

		// push-back (preallocated) [subMeshCount] entries in [subMeshes]
		for(UInt32 i = 0; i < subMeshCount; i++)
		{
			subMeshes.push_back(SubMesh3DSharedPtr::Null());
		}

		return true;
	}

	/*
	 * Update any objects and data structures that are dependent on this mesh's data. This
	 * method loops through each sub-mesh and calls its Update() method, which will ultimately
	 * update the corresponding sub-renderer in the Mesh3DRenderer attached to the same scene
	 * object (if there is one).
	 */
	void Mesh3D::UpdateAll()
	{
		for(UInt32 i = 0; i < subMeshCount; i++)
		{
			if(subMeshes[i].IsValid())
			{
				subMeshes[i]->Update();
			}
		}
	}

	/*
	 *Set the sub-mesh at [index] to be [mesh].
	 */
	void Mesh3D::SetSubMesh(SubMesh3DRef mesh, UInt32 index)
	{
		NONFATAL_ASSERT(mesh.IsValid(), "Mesh3D::SetSubMesh -> 'mesh' is null.", true);

		if(index < subMeshCount)
		{
			subMeshes[index] = mesh;
			mesh->SetContainerMesh(this);
			mesh->SetSubIndex(index);
			mesh->Update();
		}
		else
		{
			Debug::PrintError("Mesh3D::SetSubMesh -> Index out of range.");
			return;
		}
	}

	/*
	 * Get a reference to the sub-mesh at [index].
	 */
	SubMesh3DRef Mesh3D::GetSubMesh(UInt32 index)
	{
		if(index < subMeshCount)
		{
			return subMeshes[index];
		}
		else
		{
			Debug::PrintError("Mesh3D::GetSubMesh -> Index out of range.");
			return NullSubMesh3DRef;
		}
	}
}
