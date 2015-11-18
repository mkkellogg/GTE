#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "longmask.h"
#include "debug/gtedebug.h"

namespace GTE
{
	LongMask LongMaskUtil::InvertBitsForIndexMask(UInt16 index)
	{
		LongMask maskValue = IndexToMaskValue(index);
		return InvertBits(maskValue);
	}

	LongMask LongMaskUtil::InvertBits(LongMask mask)
	{
		LongMask result = 0;
		LongMask b = 0x8000000000000000L;

		for (Int32 i = 0; i < 64; i++)
		{
			if (!(b & mask))
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
		if (maskValue == 0) return 0;

		LongMask index = 0x0000000000000001L;
		Int32 count = 0;
		while (!(maskValue & index) && count < 128)
		{
			index <<= 1;
			count++;
		}

		return index;
	}

	LongMask LongMaskUtil::IndexToMaskValue(UInt16 index)
	{
		return (LongMask)0x0000000000000001L << index;
	}

	void LongMaskUtil::SetBit(LongMask * target, UInt16 index)
	{
		LongMask maskValue = IndexToMaskValue(index);
		LongMask uPtr = (LongMask)*target;
		uPtr |= (LongMask)maskValue;
		*target = uPtr;
	}

	void LongMaskUtil::ClearBit(LongMask * target, UInt16 index)
	{
		LongMask uPtr = (LongMask)*target;
		LongMask mask = InvertBitsForIndexMask(index);

		uPtr &= mask;
		*target = uPtr;
	}

	void LongMaskUtil::SetBitForMask(LongMask * target, LongMask mask)
	{
		LongMask uPtr = (LongMask)*target;
		uPtr |= mask;
		*target = uPtr;
	}

	void LongMaskUtil::ClearBitForMask(LongMask * target, LongMask mask)
	{
		LongMask uPtr = (LongMask)*target;
		LongMask iMask = InvertBits(mask);

		uPtr &= iMask;
		*target = uPtr;
	}

	Bool LongMaskUtil::IsBitSet(LongMask target, UInt16 index)
	{
		LongMask mask = IndexToMaskValue(index);
		return (target & mask) != 0L;
	}

	Bool LongMaskUtil::IsBitSetForMask(LongMask target, LongMask mask)
	{
		return (target & mask) != 0L;
	}

	LongMask LongMaskUtil::CreateLongMask()
	{
		return 0L;
	}
}
