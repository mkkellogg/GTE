#ifndef _LONGMASK_H_
#define _LONGMASK_H_

typedef unsigned long LongMask;

class LongMaskUtil
{
	public:

	static LongMask InvertBitsForIndexMask(unsigned short index);
	static LongMask InvertBits(LongMask mask);

	static LongMask MaskValueToIndex(LongMask maskValue);
	static LongMask IndexToMaskValue(unsigned short index);

	static void SetBit(LongMask * target, unsigned short index);
	static void ClearBit(LongMask * target, unsigned short index);

	static void SetBitForMask(LongMask * target, LongMask mask);
	static void ClearBitForMask(LongMask * target, LongMask mask);

	static bool IsBitSet(LongMask target, unsigned short index);
	static bool IsBitSetForMask(LongMask target,LongMask mask);

	static LongMask CreateLongMask();
};

#endif
