#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scalekeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

ScaleKeyFrame::ScaleKeyFrame() : KeyFrame()
{

}

ScaleKeyFrame::ScaleKeyFrame(float normalizedTime, float realTime,  float realTimeTicks, const Vector3& scale) : KeyFrame(normalizedTime, realTime, realTimeTicks)
{
	this->Scale = scale;
}

ScaleKeyFrame::~ScaleKeyFrame()
{

}
