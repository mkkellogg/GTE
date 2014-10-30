#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scalekeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

ScaleKeyFrame::ScaleKeyFrame() : KeyFrame()
{

}

ScaleKeyFrame::ScaleKeyFrame(float normalizedTime, float realTime,  const Vector3& scale) : KeyFrame(normalizedTime, realTime)
{
	this->Scale = scale;
}

ScaleKeyFrame::~ScaleKeyFrame()
{

}
