#include <iostream>
#include <math.h>

#include "screendesc.h"

namespace GTE
{
	ScreenDescriptor::ScreenDescriptor() : ScreenDescriptor(0, 0)
	{

	}

	ScreenDescriptor::ScreenDescriptor(Int32 screenWidth, Int32 screenHeight)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}

	ScreenDescriptor::~ScreenDescriptor()
	{

	}

	Int32 ScreenDescriptor::GetScreenWidth()
	{
		return screenWidth;
	}

	Int32 ScreenDescriptor::getScreenHeight()
	{
		return screenHeight;
	}
}

