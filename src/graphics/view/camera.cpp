#include <iostream>
#include <math.h>

#include "camera.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/graphics.h"
#include "graphics/screendesc.h"
#include "base/intmask.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"

Camera::Camera(Graphics * graphics)
{
	clearBufferMask = 0;
	this->graphics = graphics;

	projectionTransform = Transform::CreateIdentityTransform();

	Matrix4x4 proj;
	const GraphicsAttributes& graphicsAttributes = graphics->GetAttributes();

	float ratio = (float)graphicsAttributes.WindowWidth / (float)graphicsAttributes.WindowHeight;

	Transform::BuildProjectionMatrix(&proj, 65, ratio, 10, 100);

	projectionTransform->SetTo(&proj);

	//Matrix4x4 * projectionMatrix = projectionTransform->GetMatrix();
	//Transform::BuildProjectionMatrix(projectioNmatrix);
}

Camera::~Camera()
{
	SAFE_DELETE(projectionTransform);
}

void Camera::AddClearBuffer(RenderBufferType buffer)
{
	IntMaskUtil::SetBitForMask(&clearBufferMask, (unsigned int)buffer);
}

void Camera::RemoveClearBuffer(RenderBufferType buffer)
{
	IntMaskUtil::ClearBitForMask(&clearBufferMask, (unsigned int)buffer);
}

unsigned int Camera::GetClearBufferMask() const
{
	return clearBufferMask;
}

const Transform * Camera::GetProjectionTransform() const
{
	return projectionTransform;
}


