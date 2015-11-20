#ifndef _GTE_INTMASK_H_
#define _GTE_INTMASK_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	typedef UInt32 IntMask;

	class IntMaskUtil
	{
	public:

		static IntMask InvertBitsForIndexMask(UInt16 index);
		static IntMask InvertBits(IntMask index);

		static IntMask MaskValueToIndex(IntMask maskValue);
		static IntMask IndexToMaskValue(UInt16 index);

		static void SetBit(IntMask * target, UInt16 index);
		static void ClearBit(IntMask * target, UInt16 index);

		static void SetBitForMask(IntMask * target, IntMask mask);
		static void ClearBitForMask(IntMask * target, IntMask mask);

		static Bool IsBitSet(IntMask target, UInt16 index);
		static Bool IsBitSetForMask(IntMask target, IntMask mask);

		static IntMask CreateIntMask();

		static IntMask MergeMasks(IntMask a, IntMask b);
		static Bool HaveAtLeastOneInCommon(IntMask a, IntMask b);

		static void ClearMask(IntMask * mask);
		static void SetAll(IntMask * mask);
	};
}

#endif
