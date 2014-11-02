#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rotationkeyframe.h"
#include <string>

RotationKeyFrame::RotationKeyFrame() : KeyFrame()
{

}

RotationKeyFrame::RotationKeyFrame(float normalizedTime, float realTime,  float realTimeTicks, const Quaternion& rotation) : KeyFrame(normalizedTime, realTime, realTimeTicks)
{
	this->Rotation = rotation;
}

RotationKeyFrame::~RotationKeyFrame()
{

}
