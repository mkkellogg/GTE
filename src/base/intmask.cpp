#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "intmask.h"
#include "ui/debug.h"

unsigned int IntMask::InvertBitsForIndexMask(unsigned int index)
{
	unsigned int maskValue = IndexToMaskValue(index);
	return InvertBits(maskValue);
}

unsigned int IntMask::InvertBits(unsigned int a)
{
	unsigned int result = 0;
	unsigned int mask = 0x80000000;

	for(int i=0; i < 32; i++)
	{
		if(!(mask & a))
		{
			result |= 1;
		}

		result <<= 1;
		mask >>= 1;
	}

	return result;
}

unsigned int IntMask::MaskValueToIndex(unsigned int maskValue)
{
	if(maskValue == 0) return 0;

	unsigned int index=0x00000001;
	int count = 0;
	while(!(maskValue & index) && count < 64)
	{
		index <<= 1;
		count++;
	}

	return index;
}

unsigned int IntMask::IndexToMaskValue(unsigned int index)
{
	return 0x00000001 << index;
}

void IntMask::SetBitForIndexMask(unsigned int * target, unsigned short index)
{
	unsigned int maskValue = IndexToMaskValue(index);
	unsigned int uPtr = (unsigned int)*target;
	uPtr |= (unsigned int)maskValue;
	*target = uPtr;
}

void IntMask::ClearBitForIndexMask(unsigned int * target, unsigned short index)
{
	unsigned int uPtr = (unsigned int)*target;
	unsigned int mask = InvertBitsForIndexMask(index);

	uPtr &= mask;
	*target = uPtr;
}

bool IntMask::IsBitSet(unsigned int target, unsigned short index)
{
	unsigned int mask = IndexToMaskValue(index);
	return target & mask;
}
