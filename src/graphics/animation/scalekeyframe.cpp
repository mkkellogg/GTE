#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scalekeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

namespace GTE
{
	/*
	* Default constructor.
	*/
	ScaleKeyFrame::ScaleKeyFrame() : KeyFrame()
	{

	}

	/*
	 * Parameterized constructor.
	 */
	ScaleKeyFrame::ScaleKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Vector3& scale) : KeyFrame(normalizedTime, realTime, realTimeTicks)
	{
		this->Scale = scale;
	}

	/*
	 * Destructor.
	 */
	ScaleKeyFrame::~ScaleKeyFrame()
	{

	}
}
