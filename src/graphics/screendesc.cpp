#include <iostream>
#include <math.h>

#include "screendesc.h"

namespace GTE
{
	ScreenDescriptor::ScreenDescriptor() : ScreenDescriptor(0, 0)
	{

	}

	ScreenDescriptor::ScreenDescriptor(int screenWidth, int screenHeight)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}

	ScreenDescriptor::~ScreenDescriptor()
	{

	}

	int ScreenDescriptor::GetScreenWidth()
	{
		return screenWidth;
	}

	int ScreenDescriptor::getScreenHeight()
	{
		return screenHeight;
	}
}

