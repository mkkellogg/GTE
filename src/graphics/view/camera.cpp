#include <iostream>
#include <math.h>

#include "camera.h"
#include "engine.h"
#include "graphics/render/rendertarget.h"
#include "graphics/graphics.h"
#include "graphics/screendesc.h"
#include "base/intmask.h"
#include "global/global.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"

Camera::Camera()
{
	clearBufferMask = 0;
	UpdateDisplay();
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
	return projection;
}

void Camera::UpdateDisplay()
{
	Matrix4x4 proj;
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	const GraphicsAttributes& graphicsAttributes = graphics->GetAttributes();

	float ratio = (float)graphicsAttributes.WindowWidth / (float)graphicsAttributes.WindowHeight;

	Transform::BuildProjectionMatrix(proj, 65, ratio, 5, 100);
	//Transform::BuildProjectionMatrixInfiniteFar(proj, 65, ratio, 3);
	projection.SetTo(proj);
}


