#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rotationkeyframe.h"
#include <string>

RotationKeyFrame::RotationKeyFrame() : KeyFrame()
{

}

RotationKeyFrame::RotationKeyFrame(float normalizedTime, float realTime,  const Quaternion& rotation) : KeyFrame(normalizedTime, realTime)
{
	this->Rotation = rotation;
}

RotationKeyFrame::~RotationKeyFrame()
{

}
