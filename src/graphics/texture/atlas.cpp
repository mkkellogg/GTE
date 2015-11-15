#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "atlas.h"
#include "graphics/image/rawimage.h"
#include "graphics/image/imageloader.h"
#include "object/enginetypes.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"

namespace GTE
{
	Atlas::Atlas(TextureRef texture, Bool createFirstFullFrame)
	{
		this->texture = texture;
		imageCount = 0;

		if(createFirstFullFrame)
		{
			AddImageDescriptor(0, 1, 1, 0);
		}
	}

	Atlas::~Atlas()
	{

	}

	void Atlas::AddImageDescriptor(Real left, Real top, Real right, Real bottom)
	{
		ImageDescriptor newDesc(left, top, right, bottom);
		imageDescriptors.push_back(newDesc);
		imageCount++;
	}

	Atlas::ImageDescriptor* Atlas::GetImageDescriptor(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < imageCount, "Atlas::GetImageDescriptor -> 'index' is out of range.", nullptr, true);

		return &imageDescriptors[index];
	}

	TextureRef Atlas::GetTexture()
	{
		return texture;
	}

	Atlas* Atlas::CreateGridAtlas(TextureRef texture, Real left, Real top, Real right, Real bottom, UInt32 xCount, UInt32 yCount, Bool reverseX, Bool reverseY)
	{
		Atlas * atlas = new(std::nothrow) Atlas(texture, false);
		ASSERT(atlas != nullptr, "Atlas::CreateGridAtlas -> Unable to allocate atlas.");

		Real width = right - left;
		Real height = top - bottom;

		Real xBlockSize = width / (Real)xCount;
		Real yBlockSize = height / (Real)yCount;

		Int32 xInc = 1;
		Int32 yInc = 1;

		Int32 xStart = 0;
		Int32 yStart = 0;

		Int32 xFinish = (Int32)xCount;
		Int32 yFinish = (Int32)yCount;

		if(reverseX)
		{
			xInc = -1;
			xStart = xCount - 1;
			xFinish = -1;
		}

		if(reverseY)
		{
			yInc = -1;
			yStart = yCount - 1;
			yFinish = -1;
		}

		for(Int32 y = yStart; y != yFinish; y += yInc)
		{
			for(Int32 x = xStart; x != xFinish; x += xInc)
			{
				Real currentLeft = left + xBlockSize * x;
				Real currentTop = bottom + yBlockSize * (y + 1);
				Real currentRight = currentLeft + xBlockSize;
				Real currentBottom = currentTop - yBlockSize;

				atlas->AddImageDescriptor(currentLeft, currentTop, currentRight, currentBottom);
			}
		}

		return atlas;
	}
}
