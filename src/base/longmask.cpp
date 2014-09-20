#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "longmask.h"
#include "ui/debug.h"

LongMask LongMaskUtil::InvertBitsForIndexMask(unsigned short index)
{
	LongMask maskValue = IndexToMaskValue(index);
	return InvertBits(maskValue);
}

LongMask LongMaskUtil::InvertBits(LongMask mask)
{
	LongMask result = 0;
	LongMask b = 0x8000000000000000L;

	for(int i=0; i < 64; i++)
	{
		if(!(b & mask))
		{
			result |= 1;
		}

		result <<= 1;
		b >>= 1;
	}

	return result;
}

LongMask LongMaskUtil::MaskValueToIndex(LongMask maskValue)
{
	if(maskValue == 0) return 0;

	LongMask index=0x0000000000000001L;
	int count = 0;
	while(!(maskValue & index) && count < 128)
	{
		index <<= 1;
		count++;
	}

	return index;
}

LongMask LongMaskUtil::IndexToMaskValue(unsigned short index)
{
	return 0x0000000000000001L << index;
}

void LongMaskUtil::SetBit(LongMask * target, unsigned short index)
{
	unsigned int maskValue = IndexToMaskValue(index);
	unsigned int uPtr = (unsigned int)*target;
	uPtr |= (unsigned int)maskValue;
	*target = uPtr;
}

void LongMaskUtil::ClearBit(LongMask * target, unsigned short index)
{
	unsigned int uPtr = (unsigned int)*target;
	unsigned int mask = InvertBitsForIndexMask(index);

	uPtr &= mask;
	*target = uPtr;
}

void LongMaskUtil::SetBitForMask(LongMask * target, LongMask mask)
{
	unsigned int uPtr = (unsigned int)*target;
	uPtr |= mask;
	*target = uPtr;
}

void LongMaskUtil::ClearBitForMask(LongMask * target, LongMask mask)
{
	unsigned int uPtr = (unsigned int)*target;
	unsigned int iMask = InvertBits(mask);

	uPtr &= iMask;
	*target = uPtr;
}

bool LongMaskUtil::IsBitSet(LongMask target, unsigned short index)
{
	unsigned int mask = IndexToMaskValue(index);
	return target & mask;
}

bool LongMaskUtil::IsBitSetForMask(LongMask target,LongMask mask)
{
	return target & mask;
}

LongMask LongMaskUtil::CreateLongMask()
{
	return 0L;
}
