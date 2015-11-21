#include "engine.h"
#include "intmask.h"
#include "debug/gtedebug.h"

namespace GTE
{
	IntMask IntMaskUtil::InvertBitsForIndexMask(UInt16 index)
	{
		IntMask maskValue = IndexToMaskValue(index);
		return InvertBits(maskValue);
	}

	IntMask IntMaskUtil::InvertBits(IntMask a)
	{
		return ~a;
	}

	IntMask IntMaskUtil::MaskValueToIndex(IntMask maskValue)
	{
		if (maskValue == 0) return 0;

		IntMask index = 0x00000001;
		Int32 count = 0;
		while (!(maskValue & index) && count < 32)
		{
			index <<= 1;
			count++;
		}

		return index;
	}

	IntMask IntMaskUtil::IndexToMaskValue(UInt16 index)
	{
		return 0x00000001 << index;
	}

	void IntMaskUtil::SetBit(IntMask * target, UInt16 index)
	{
		IntMask maskValue = IndexToMaskValue(index);
		IntMask uPtr = (IntMask)*target;
		uPtr |= (IntMask)maskValue;
		*target = uPtr;
	}

	void IntMaskUtil::ClearBit(IntMask * target, UInt16 index)
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

	Bool IntMaskUtil::IsBitSet(IntMask target, UInt16 index)
	{
		IntMask mask = IndexToMaskValue(index);
		return (target & mask) != 0;
	}

	Bool IntMaskUtil::IsBitSetForMask(IntMask target, IntMask mask)
	{
		return (target & mask) != 0;
	}

	IntMask IntMaskUtil::CreateIntMask()
	{
		return 0;
	}

	IntMask IntMaskUtil::MergeMasks(IntMask a, IntMask b)
	{
		return a | b;
	}

	Bool IntMaskUtil::HaveAtLeastOneInCommon(IntMask a, IntMask b)
	{
		return (a & b) != 0;
	}

	void IntMaskUtil::ClearMask(IntMask * mask)
	{
		*mask = 0;
	}

	void IntMaskUtil::SetAll(IntMask * mask)
	{
		*mask = (UInt32)~0;
	}
}
