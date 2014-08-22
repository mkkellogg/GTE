#include <iostream>
#include <math.h>
#include "viewsystem.h"
#include "global/global.h"
#include "ui/debug.h"

ViewSystem::ViewSystem(const Graphics * graphics)
{
	this->graphics = graphics;
}

ViewSystem::~ViewSystem()
{
	DestroyCameras();
}

void ViewSystem::DestroyCameras()
{
	for(int i = 0; i< (int)cameras.size(); i++)
	{
		Camera * camera = cameras[i];
		delete camera;
	}
	cameras.clear();
}

void ViewSystem::DestroyCamera(Camera * camera)
{
	delete camera;
}

int ViewSystem::CameraCount() const
{
	return (int)cameras.size();
}

const Camera * ViewSystem::GetCamera(int index) const
{
	if(index >=0 && index < (int)cameras.size())
	{
		return (const Camera *)cameras[index];
	}

	Debug::PrintError("ViewSystem::GetCamera() -> index out of range.");
	return NULL;
}


