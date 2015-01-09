#ifndef _GTE_INTMASK_H_
#define _GTE_INTMASK_H_

typedef unsigned int IntMask;

class IntMaskUtil
{
	public:

	static IntMask InvertBitsForIndexMask(IntMask index);
	static IntMask InvertBits(IntMask index);

	static IntMask MaskValueToIndex(IntMask maskValue);
	static IntMask IndexToMaskValue(IntMask index);

	static void SetBit(IntMask * target, unsigned short index);
	static void ClearBit(IntMask * target, unsigned short index);

	static void SetBitForMask(IntMask * target, IntMask mask);
	static void ClearBitForMask(IntMask * target, IntMask mask);

	static bool IsBitSet(IntMask target, unsigned short index);
	static bool IsBitSetForMask(IntMask target,IntMask mask);

	static IntMask CreateIntMask();
};

#endif
