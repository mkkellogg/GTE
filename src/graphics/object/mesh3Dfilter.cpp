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
#include "mesh3Dfilter.h"
#include "mesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	* Default constructor.
	*/
	Mesh3DFilter::Mesh3DFilter()
	{
		castShadows = false;
		receiveShadows = false;
		useCustomShadowVolumeOffset = false;
		shadowVolumeOffset = 0.0f;
		doBackSetShadowVolume = true;
	}

	/*
	 * Cleanup.
	 */
	Mesh3DFilter::~Mesh3DFilter()
	{

	}

	/*
	 * Set this mesh filter's target mesh.
	 */
	void Mesh3DFilter::SetMesh3D(Mesh3DRef mesh)
	{
		this->mesh = mesh;

		if (this->sceneObject.IsValid())
		{
			sceneObject->NotifyNewMesh3D();
		}
	}

	/*
	* Remove the target mesh for this mesh filter.
	*/
	Bool Mesh3DFilter::RemoveMesh3D()
	{
		NONFATAL_ASSERT_RTRN(mesh.IsValid(), "Mesh3DFilter::RemoveMesh3D -> Mesh filter has no target mesh.", false, true);
		this->mesh = Mesh3DSharedPtr::Null();

		return true;
	}

	/*
	 * Get the target mesh for this mesh filter.
	 */
	Mesh3DRef Mesh3DFilter::GetMesh3D()
	{
		return mesh;
	}

	/*
	 * Specify whether or not [mesh] should cast shadows.
	 */
	void Mesh3DFilter::SetCastShadows(Bool castShadows)
	{
		this->castShadows = castShadows;
	}

	/*
	 * Should [mesh] cast shadows?
	 */
	Bool Mesh3DFilter::GetCastShadows() const
	{
		return castShadows;
	}

	/*
	 * Specify whether or not [mesh] should receive shadows.
	 */
	void Mesh3DFilter::SetReceiveShadows(Bool receiveShadows)
	{
		this->receiveShadows = receiveShadows;
	}

	/*
	 * Should [mesh] receive shadows?
	 */
	Bool Mesh3DFilter::GetReceiveShadows() const
	{
		return receiveShadows;
	}

	/*
	* Specify whether or not a custom shadow volume offset be used when rendering the shadow for [mesh]?
	*/
	void Mesh3DFilter::SetUseCustomShadowVolumeOffset(Bool useCustomShadowVolumeOffset)
	{
		this->useCustomShadowVolumeOffset = useCustomShadowVolumeOffset;
	}

	/*
	* Should a custom shadow volume offset be used when rendering the shadow for [mesh]?
	*/
	Bool Mesh3DFilter::GetUseCustomShadowVolumeOffset() const
	{
		return useCustomShadowVolumeOffset;
	}

	/*
	* Set the custom shadow volume offset.
	*/
	void Mesh3DFilter::SetCustomShadowVolumeOffset(Real shadowVolumeOffset)
	{
		this->shadowVolumeOffset = shadowVolumeOffset;
	}

	/*
	* Get the custom shadow volume offset.
	*/
	Real Mesh3DFilter::GetCustomShadowVolumeOffset() const
	{
		return shadowVolumeOffset;
	}

	/*
	* Set the [doBackSetShadowVolume] member boolean.
	*/
	void Mesh3DFilter::SetUseBackSetShadowVolume(Bool use)
	{
		doBackSetShadowVolume = use;
	}

	/*
	* Access [doBackSetShadowVolume] member boolean.
	*/
	Bool Mesh3DFilter::GetUseBackSetShadowVolume() const
	{
		return doBackSetShadowVolume;
	}
}

