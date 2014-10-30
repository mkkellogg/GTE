#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "translationkeyframe.h"
#include "geometry/vector/vector3.h"
#include <string>

TranslationKeyFrame::TranslationKeyFrame() : KeyFrame()
{

}

TranslationKeyFrame::TranslationKeyFrame(float normalizedTime, float realTime,  const Vector3& translation) : KeyFrame(normalizedTime, realTime)
{
	this->Translation = translation;
}

TranslationKeyFrame::~TranslationKeyFrame()
{

}
