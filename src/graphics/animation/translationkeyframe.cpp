#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "translationkeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

namespace GTE
{
	/*
	* Default constructor.
	*/
	TranslationKeyFrame::TranslationKeyFrame() : KeyFrame()
	{

	}

	/*
	 * Parameterized constructor..
	 */
	TranslationKeyFrame::TranslationKeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks, const Vector3& translation) : KeyFrame(normalizedTime, realTime, realTimeTicks)
	{
		this->Translation = translation;
	}

	/*
	 * Destructor.
	 */
	TranslationKeyFrame::~TranslationKeyFrame()
	{

	}
}
