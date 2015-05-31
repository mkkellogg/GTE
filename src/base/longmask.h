#ifndef _GTE_LONGMASK_H_
#define _GTE_LONGMASK_H_

#include "object/enginetypes.h"
#include "global/global.h"

namespace GTE
{
	typedef unsigned long LongMask;

	class LongMaskUtil
	{
	public:

		static LongMask InvertBitsForIndexMask(UInt16 index);
		static LongMask InvertBits(LongMask mask);

		static LongMask MaskValueToIndex(LongMask maskValue);
		static LongMask IndexToMaskValue(UInt16 index);

		static void SetBit(LongMask * target, UInt16 index);
		static void ClearBit(LongMask * target, UInt16 index);

		static void SetBitForMask(LongMask * target, LongMask mask);
		static void ClearBitForMask(LongMask * target, LongMask mask);

		static bool IsBitSet(LongMask target, UInt16 index);
		static bool IsBitSetForMask(LongMask target, LongMask mask);

		static LongMask CreateLongMask();
	};
}

#endif
