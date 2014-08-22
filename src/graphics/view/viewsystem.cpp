#include <iostream>
#include <math.h>
#include "viewsystem.h"
#include "global/global.h"

ViewSystem::ViewSystem(const Graphics * graphics)
{
	this->graphics = graphics;
	activeCamera = new Camera(this);
}

ViewSystem::~ViewSystem()
{
	delete activeCamera;
}

Camera * ViewSystem::GetActiveCamera()
{
	return activeCamera;
}

