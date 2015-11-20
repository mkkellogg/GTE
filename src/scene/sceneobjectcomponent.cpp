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

	SceneObjectComponentProcessingDescriptor& SceneObjectComponent::GetProcessingDescriptor()
	{
		return processingDescriptor;
	}

	void SceneObjectComponent::SetSceneObject(SceneObjectRef sceneObject)
	{
		this->sceneObject = sceneObject;
	}

	SceneObjectRef SceneObjectComponent::GetSceneObject()
	{
		return sceneObject;
	}

	void SceneObjectComponent::Awake()
	{

	}

	void SceneObjectComponent::Start()
	{

	}

	void SceneObjectComponent::Update()
	{

	}

	void SceneObjectComponent::PreRender()
	{

	}

	void SceneObjectComponent::WillRender()
	{

	}
}
