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

Mesh3DFilter::Mesh3DFilter()
{

}

Mesh3DFilter::~Mesh3DFilter()
{

}

void Mesh3DFilter::SetMesh3D(Mesh3DRef mesh)
{
	this->mesh = mesh;

	if(this->sceneObject.IsValid())
	{
		sceneObject->NotifyNewMesh3D();
	}
}

Mesh3DRef Mesh3DFilter::GetMesh3D()
{
	return mesh;
}
