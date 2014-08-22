#ifndef _INTMASK_H_
#define _INTMASK_H_

class IntMask
{
	public:

	static unsigned int InvertBitsForIndexMask(unsigned int index);
	static unsigned int InvertBits(unsigned int index);

	static unsigned int MaskValueToIndex(unsigned int maskValue);
	static unsigned int IndexToMaskValue(unsigned int index);

	static void SetBitForIndexMask(unsigned int * target, unsigned short index);
	static void ClearBitForIndexMask(unsigned int * target, unsigned short index);

	static void SetBit(unsigned int * target, unsigned int mask);
	static void ClearBit(unsigned int * target, unsigned int mask);

	static bool IsBitSet(unsigned int target, unsigned short index);
	static bool IsBitSetForMask(unsigned int target,unsigned int mask);
};

#endif
