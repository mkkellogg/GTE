#ifndef _GTE_INTMASK_H_
#define _GTE_INTMASK_H_

namespace GTE
{
	typedef unsigned int IntMask;

	class IntMaskUtil
	{
	public:

		static IntMask InvertBitsForIndexMask(unsigned short index);
		static IntMask InvertBits(IntMask index);

		static IntMask MaskValueToIndex(IntMask maskValue);
		static IntMask IndexToMaskValue(unsigned short index);

		static void SetBit(IntMask * target, unsigned short index);
		static void ClearBit(IntMask * target, unsigned short index);

		static void SetBitForMask(IntMask * target, IntMask mask);
		static void ClearBitForMask(IntMask * target, IntMask mask);

		static bool IsBitSet(IntMask target, unsigned short index);
		static bool IsBitSetForMask(IntMask target, IntMask mask);

		static IntMask CreateIntMask();

		static IntMask MergeMasks(IntMask a, IntMask b);
		static bool HaveAtLeastOneInCommon(IntMask a, IntMask b);

		static void ClearMask(IntMask * mask);
		static void SetAll(IntMask * mask);
	};
}

#endif
