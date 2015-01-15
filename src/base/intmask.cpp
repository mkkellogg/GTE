#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "intmask.h"
#include "debug/debug.h"

IntMask IntMaskUtil::InvertBitsForIndexMask(IntMask index)
{
	IntMask maskValue = IndexToMaskValue(index);
	return InvertBits(maskValue);
}

IntMask IntMaskUtil::InvertBits(IntMask a)
{
	IntMask result = 0;
	IntMask mask = 0x80000000;

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

IntMask IntMaskUtil::MaskValueToIndex(IntMask maskValue)
{
	if(maskValue == 0) return 0;

	IntMask index=0x00000001;
	int count = 0;
	while(!(maskValue & index) && count < 64)
	{
		index <<= 1;
		count++;
	}

	return index;
}

IntMask IntMaskUtil::IndexToMaskValue(IntMask index)
{
	return 0x00000001 << index;
}

void IntMaskUtil::SetBit(IntMask * target, unsigned short index)
{
	IntMask maskValue = IndexToMaskValue(index);
	IntMask uPtr = (IntMask)*target;
	uPtr |= (IntMask)maskValue;
	*target = uPtr;
}

void IntMaskUtil::ClearBit(IntMask * target, unsigned short index)
{
	IntMask uPtr = (IntMask)*target;
	IntMask mask = InvertBitsForIndexMask(index);

	uPtr &= mask;
	*target = uPtr;
}

void IntMaskUtil::SetBitForMask(IntMask * target, IntMask mask)
{
	IntMask uPtr = (IntMask)*target;
	uPtr |= mask;
	*target = uPtr;
}

void IntMaskUtil::ClearBitForMask(IntMask * target, IntMask mask)
{
	IntMask uPtr = (IntMask)*target;
	IntMask iMask = InvertBits(mask);

	uPtr &= iMask;
	*target = uPtr;
}

bool IntMaskUtil::IsBitSet(IntMask target, unsigned short index)
{
	IntMask mask = IndexToMaskValue(index);
	return target & mask;
}

bool IntMaskUtil::IsBitSetForMask(IntMask target,IntMask mask)
{
	return target & mask;
}

IntMask IntMaskUtil::CreateIntMask()
{
	return 0;
}
