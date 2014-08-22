#include <iostream>
#include <math.h>
#include "camera.h"
#include "viewsystem.h"
#include "global/global.h"

Camera::Camera(const ViewSystem * viewSystem)
{
	this->viewSystem = viewSystem;

	modelViewTransform = Transform::CreateIdentityTransform();
	projectionTransform = Transform::CreateIdentityTransform();
	mvpTransform = Transform::CreateIdentityTransform();

	//Matrix4x4 * projectionMatrix = projectionTransform->GetMatrix();
	//Transform::BuildProjectionMatrix(projectioNmatrix);
}

Camera::~Camera()
{
	SAFE_DELETE(modelViewTransform);
	SAFE_DELETE(projectionTransform);
	SAFE_DELETE(mvpTransform);
}


const Transform * Camera::GetModelViewTransform() const
{
	return modelViewTransform;
}

const Transform * Camera::GetProjectionTransform() const
{
	return projectionTransform;
}

const Transform * Camera::GetMVPTransform() const
{
	return mvpTransform;
}

