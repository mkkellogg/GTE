#include "scalekeyframe.h"
#include "geometry/vector/vector3.h"

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
	ScaleKeyFrame::ScaleKeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks, const Vector3& scale) : KeyFrame(normalizedTime, realTime, realTimeTicks)
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
