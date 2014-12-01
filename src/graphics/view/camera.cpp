#include <iostream>
#include <math.h>

#include "camera.h"
#include "graphics/render/rendertarget.h"
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

	Matrix4x4 proj;
	const GraphicsAttributes& graphicsAttributes = graphics->GetAttributes();

	float ratio = (float)graphicsAttributes.WindowWidth / (float)graphicsAttributes.WindowHeight;

	Transform::BuildProjectionMatrix(proj, 65, ratio, 10, 100);

	projectionTransform.SetTo(proj);
}

Camera::~Camera()
{

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

const Transform& Camera::GetProjectionTransform() const
{
	return projectionTransform;
}


