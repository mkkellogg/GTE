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
#include "debug/gtedebug.h"

/*
 * Default constructor.
 */
Mesh3DFilter::Mesh3DFilter()
{
	castShadows = false;
	receiveShadows = false;
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

	if(this->sceneObject.IsValid())
	{
		sceneObject->NotifyNewMesh3D();
	}
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
void Mesh3DFilter::SetCastShadows(bool castShadows)
{
	this->castShadows = castShadows;
}

/*
 * Should [mesh] cast shadows?
 */
bool Mesh3DFilter::GetCastShadows() const
{
	return castShadows;
}

/*
 * Specify whether or not [mesh] should receive shadows.
 */
void Mesh3DFilter::SetReceiveShadows(bool receiveShadows)
{
	this->receiveShadows = receiveShadows;
}

/*
 * Should [mesh] receive shadows?
 */
bool Mesh3DFilter::GetReceiveShadows() const
{
	return receiveShadows;
}

