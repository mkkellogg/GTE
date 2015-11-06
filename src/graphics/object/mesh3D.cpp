#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <unordered_map>
#include <functional>
#include <vector>

#include "engine.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
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
		if (subMeshCount <= 0)subMeshCount = 1;
		this->subMeshCount = subMeshCount;
		lightCullType = LightCullType::SphereOfInfluence;
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
		for (UInt32 i = 0; i < subMeshCount; i++)
		{
			subMeshes.push_back(SubMesh3DSharedPtr::Null());
		}

		return true;
	}

	/*
	 * Calculate the collective sphere (spheroid) of influence for all the sub-meshes in this
	 * instance of Mesh3D,
	 *
	 * The sphere of influence is a less than perfect way to cull meshes from being rendered for
	 * lights that are too far away. It is basically a sphere that is guaranteed to contain all the
	 * sub-meshes in this Mesh3D object.
	 */
	void Mesh3D::CalculateSphereOfInfluence()
	{
		Point3 average;
		UInt32 validSubMeshes = 0;

		// loop through each sub-mesh to calculate average center position
		for (UInt32 i = 0; i < subMeshCount; i++)
		{
			SubMesh3DSharedPtr subMesh = subMeshes[i];
			if (subMesh.IsValid())
			{
				const Point3 temp = subMesh->GetCenter();
				average.x += temp.x;
				average.y += temp.y;
				average.z += temp.z;
				validSubMeshes++;
			}
		}

		// calculate the average position
		if (validSubMeshes > 0)
		{
			average.x /= (Real)validSubMeshes;
			average.y /= (Real)validSubMeshes;
			average.z /= (Real)validSubMeshes;
		}

		center = average;

		Real maxSoiX = 0;
		Real maxSoiY = 0;
		Real maxSoiZ = 0;

		// loop through each sub-mesh and determine the maximum distance away from
		// the averaged center it extends along each axis. Store those values
		// in [maxSoiX], [maxSoiY], and [maxSoiX] respectively.
		for (UInt32 i = 0; i < subMeshCount; i++)
		{
			SubMesh3DSharedPtr subMesh = subMeshes[i];
			if (subMesh.IsValid())
			{
				const Point3 temp = subMesh->GetCenter();

				// calculate the distance from the averaged center to the sub-meshes center
				Real offsetX = GTEMath::Abs(center.x - temp.x);
				Real offsetY = GTEMath::Abs(center.y - temp.y);
				Real offsetZ = GTEMath::Abs(center.z - temp.z);

				// use the sphere of influence values already calculated for each sub-mesh
				// and add them to the the center offset
				Real soiX = offsetX + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().x);
				Real soiY = offsetY + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().y);
				Real soiZ = offsetZ + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().z);

				// compare to max values and update if necessary
				if (soiX > maxSoiX)maxSoiX = soiX;
				if (soiY > maxSoiY)maxSoiY = soiY;
				if (soiZ > maxSoiZ)maxSoiZ = soiZ;
			}
		}

		// set the final values for each axis
		sphereOfInfluenceX.Set(maxSoiX, 0, 0);
		sphereOfInfluenceY.Set(0, maxSoiY, 0);
		sphereOfInfluenceZ.Set(0, 0, maxSoiZ);
	}

	/*
	 * Update any objects and data structures that are dependent on this mesh's data. This
	 * method loops through each sub-mesh and calls its Update() method, which will ultimately
	 * update the corresponding sub-renderer in the Mesh3DRenderer attached to the same scene
	 * object (if there is one).
	 */
	void Mesh3D::Update()
	{
		for (UInt32 i = 0; i < subMeshCount; i++)
		{
			if (subMeshes[i].IsValid())
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

		if (index < subMeshCount)
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
		if (index < subMeshCount)
		{
			return subMeshes[index];
		}
		else
		{
			Debug::PrintError("Mesh3D::GetSubMesh -> Index out of range.");
			return NullSubMesh3DRef;
		}
	}

	/*
	 * Get the calculated and averaged center of all sub-meshes contained by this
	 * instance.
	 */
	const Point3& Mesh3D::GetCenter() const
	{
		return center;
	}

	/*
	 * Get the size of the sphere of influence along the local x-axis.
	 */
	const Vector3& Mesh3D::GetSphereOfInfluenceX() const
	{
		return sphereOfInfluenceX;
	}

	/*
	 * Get the size of the sphere of influence along the local y-axis.
	 */
	const Vector3& Mesh3D::GetSphereOfInfluenceY() const
	{
		return sphereOfInfluenceY;
	}

	/*
	 * Get the size of the sphere of influence along the local z-axis.
	 */
	const Vector3& Mesh3D::GetSphereOfInfluenceZ() const
	{
		return sphereOfInfluenceZ;
	}

	/*
	 * Get the type of light culling that should be used for this mesh.
	 */
	LightCullType Mesh3D::GetLightCullType() const
	{
		return lightCullType;
	}
}
