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

//forward declarations
class EngineObjectManager;

#include "object/engineobject.h"
#include "object/sceneobjectcomponent.h"
#include "object/enginetypes.h"
#include <vector>

class Mesh3DFilter : public SceneObjectComponent
{
	// Since this ultimately derives from EngineObject, we make this class
	// a friend of EngineObjectManager, and the constructor & destructor
	// protected so its life-cycle can be handled completely by EngineObjectManager.
	friend class EngineObjectManager;

	Mesh3DRef mesh;

	// should [meah] cast shadows?
	bool castShadows;
	// should this mesh receive shadows?
	bool receiveShadows;

    Mesh3DFilter();
    ~Mesh3DFilter();

	public:

    void SetMesh3D(Mesh3DRef mesh);
    Mesh3DRef GetMesh3D();

    void SetCastShadows(bool castShadows);
   bool GetCastShadows();
   void SetReceiveShadows(bool receiveShadows);
   bool GetReceiveShadows();
};

#endif

