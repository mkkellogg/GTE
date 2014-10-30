#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyframe.h"
#include <string>

KeyFrame::KeyFrame()
{
	NormalizedTime = 0;
	RealTime = 0;
}

KeyFrame::KeyFrame(float normalizedTime, float realTime)
{
	this->NormalizedTime = normalizedTime;
	this->RealTime = realTime;
}

KeyFrame::~KeyFrame()
{

}
