#include <iostream>
#include <math.h>
#include "camera.h"
#include "base/intmask.h"
#include "viewsystem.h"
#include "global/global.h"

Camera::Camera()
{
	modelViewTransform = Transform::CreateIdentityTransform();
	projectionTransform = Transform::CreateIdentityTransform();
	mvpTransform = Transform::CreateIdentityTransform();

	clearBufferMask = 0;

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

void Camera::AddClearBuffer(RenderBufferType buffer)
{
	IntMask::SetBit(&clearBufferMask, (unsigned int)buffer);
}

void Camera::RemoveClearBuffer(RenderBufferType buffer)
{
	IntMask::ClearBit(&clearBufferMask, (unsigned int)buffer);
}

unsigned int Camera::GetClearBufferMask() const
{
	return clearBufferMask;
}

