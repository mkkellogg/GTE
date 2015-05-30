#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyframe.h"
#include <string>

namespace GTE
{
	/*
	* Default constructor.
	*/
	KeyFrame::KeyFrame()
	{
		NormalizedTime = 0;
		RealTime = 0;
		RealTimeTicks = 1;
	}

	/*
	 * Parameterized constructor.
	 */
	KeyFrame::KeyFrame(float normalizedTime, float realTime, float realTimeTicks)
	{
		this->NormalizedTime = normalizedTime;
		this->RealTime = realTime;
		this->RealTimeTicks = realTimeTicks;
	}

	/*
	 * Destructor.
	 */
	KeyFrame::~KeyFrame()
	{

	}
}
