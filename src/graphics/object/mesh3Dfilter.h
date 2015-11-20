/*
 * class: Mesh3DFilter
 *
 * author: Mark Kellogg
 *
 * Mesh3DFilter is a SceneObjectComponent that is a container for a Mesh3D
 * instance.
 */

#ifndef _GTE_MESH3D_FILTER_H_
#define _GTE_MESH3D_FILTER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "scene/sceneobjectcomponent.h"

#include <vector>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;

	class Mesh3DFilter : public SceneObjectComponent
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		Mesh3DSharedPtr mesh;

		// should [mesh] cast shadows?
		Bool castShadows;
		// should this mesh receive shadows?
		Bool receiveShadows;
		// use custom shadow volume offset
		Bool useCustomShadowVolumeOffset;
		// custom shadow volume offset
		Real shadowVolumeOffset;
		// use an alternate shadow volume technique where the front caps are made up
		// up the back-facing triangles
		Bool doBackSetShadowVolume;


		Mesh3DFilter();
		~Mesh3DFilter();

	public:

		void SetMesh3D(Mesh3DRef mesh);
		Bool RemoveMesh3D();
		Mesh3DRef GetMesh3D();

		void SetCastShadows(Bool castShadows);
		Bool GetCastShadows() const;
		void SetReceiveShadows(Bool receiveShadows);
		Bool GetReceiveShadows() const;
		void SetUseCustomShadowVolumeOffset(Bool useCustomShadowVolumeOffset);
		Bool GetUseCustomShadowVolumeOffset() const;
		void SetCustomShadowVolumeOffset(Real shadowVolumeOffset);
		Real GetCustomShadowVolumeOffset() const;
		void SetUseBackSetShadowVolume(Bool use);
		Bool GetUseBackSetShadowVolume() const;
	};
}

#endif

