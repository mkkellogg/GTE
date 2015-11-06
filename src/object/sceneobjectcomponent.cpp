#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "sceneobjectcomponent.h"

namespace GTE
{
	SceneObjectComponent::SceneObjectComponent()
	{

	}

	SceneObjectComponent::~SceneObjectComponent()
	{

	}

	SceneObjectSharedPtr SceneObjectComponent::GetSceneObject()
	{
		return sceneObject;
	}

	void SceneObjectComponent::Update()
	{

	}
}
