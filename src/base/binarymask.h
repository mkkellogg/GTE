#ifndef _GTE_INTMASK_H_
#define _GTE_INTMASK_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	typedef UInt32 IntMask;
	typedef UInt64 LongMask;
	
	template <class T> class BinaryMaskUtil
	{
	public:

		static T InvertBitsForIndexMask(UInt16 index)
		{
			T maskValue = IndexToMaskValue(index);
			return InvertBits(maskValue);
		}

		static T InvertBits(T a)
		{
			return ~a;
		}

		static T MaskValueToIndex(T maskValue)
		{
			if(maskValue == 0) return 0;

			T index = (T)0x00000001;
			Int32 count = 0;
			while(!(maskValue & index) && count < 32)
			{
				index <<= 1;
				count++;
			}

			return index;
		}

		static T IndexToMaskValue(UInt16 index)
		{
			return (T)0x00000001 << index;
		}

		static void SetBit(T * target, UInt16 index)
		{
			T maskValue = IndexToMaskValue(index);
			T uPtr = (T)*target;
			uPtr |= (T)maskValue;
			*target = uPtr;
		}

		static void ClearBit(T * target, UInt16 index)
		{
			T uPtr = (T)*target;
			T mask = InvertBitsForIndexMask(index);

			uPtr &= mask;
			*target = uPtr;
		}

		static void SetBitForMask(T * target, T mask)
		{
			T uPtr = (T)*target;
			uPtr |= mask;
			*target = uPtr;
		}

		static void ClearBitForMask(T * target, T mask)
		{
			T uPtr = (T)*target;
			T iMask = InvertBits(mask);

			uPtr &= iMask;
			*target = uPtr;
		}

		static Bool IsBitSet(T target, UInt16 index)
		{
			T mask = IndexToMaskValue(index);
			return (target & mask) != 0;
		}

		static Bool IsBitSetForMask(T target, T mask)
		{
			return (target & mask) != 0;
		}

		static T CreateT()
		{
			return 0;
		}

		static T MergeMasks(T a, T b)
		{
			return a | b;
		}

		static Bool HaveAtLeastOneInCommon(T a, T b)
		{
			return (a & b) != 0;
		}

		static void ClearMask(T * mask)
		{
			*mask = 0;
		}

		static void SetAll(T * mask)
		{
			*mask = (T)~0;
		}

		static T CreateMask()
		{
			return (T)0;
		}
	};

	typedef BinaryMaskUtil<IntMask> IntMaskUtil;
	typedef BinaryMaskUtil<LongMask> LongMaskUtil;
}

#endif
