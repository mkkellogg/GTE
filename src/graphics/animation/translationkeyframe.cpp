#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "translationkeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

/*
 * Default constructor.
 */
TranslationKeyFrame::TranslationKeyFrame() : KeyFrame()
{

}

/*
 * Parameterized constructor..
 */
TranslationKeyFrame::TranslationKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Vector3& translation) : KeyFrame(normalizedTime, realTime, realTimeTicks)
{
	this->Translation = translation;
}

/*
 * Destructor.
 */
TranslationKeyFrame::~TranslationKeyFrame()
{

}
