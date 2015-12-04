/*
 * class: BaseVector
 *
 * author: Mark Kellogg
 *
 * Forms the basis of several vertex attribute classes (Point3, Vector3, Color4, ...) and contains
 * functionality that is shared by all of them.
 */

#ifndef _GTE_BASEVECTOR_H_
#define _GTE_BASEVECTOR_H_

#include "engine.h"
#include "basevectortraits.h"
#include "global/constants.h"
#include "global/global.h"


#ifdef _GTE_Real_DoublePrecision
#define BaseVector4_QuickCopy(source, dest)		\
{												\
	*(dest) = *(source);(source)++;(dest)++;	\
	*(dest) = *(source);(source)++;(dest)++;	\
	*(dest) = *(source);(source)++;(dest)++;	\
	*(dest) = *(source);(source)-=3;(dest)-=3;	\
}
#else
#define BaseVector4_QuickCopy(source, dest)														\
{																								\
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) += 2;(dest) += 2; \
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) -= 2;(dest) -= 2;	\
}
#endif

#ifdef _GTE_Real_DoublePrecision
#define BaseVector4_QuickCopy_IncDest(source, dest)		\
{														\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = *(source);(source)-=3;(dest)++;			\
}
#else
#define BaseVector4_QuickCopy_IncDest(source, dest)												\
{																								\
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) += 2;(dest) += 2; \
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) -= 2;(dest) += 2;	\
}
#endif

#ifdef _GTE_Real_DoublePrecision
#define BaseVector4_QuickCopy_ZeroW(source, dest)	\
{													\
	*(dest) = *(source);(source)++;(dest)++;		\
	*(dest) = *(source);(source)++;(dest)++;		\
	*(dest) = *(source);(source)++;(dest)++;		\
	*(dest) = 0;(source)-=3;(dest)-=3;				\
}
#else
#define BaseVector4_QuickCopy_ZeroW(source, dest)												\
{																								\
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) += 2;(dest) += 2; \
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) -= 2;(dest) ++;   \
	  *((GTE::Real *)(dest)) = 0;(dest) -=3;													\
}
#endif

#ifdef _GTE_Real_DoublePrecision
#define BaseVector4_QuickCopy_ZeroW_IncDest(source, dest)\
{														\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = *(source);(source)++;(dest)++;			\
	*(dest) = 0;(source)-=3;(dest)++;					\
}
#else
#define BaseVector4_QuickCopy_ZeroW_IncDest(source, dest)										\
{																								\
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) += 2;(dest) += 2; \
	  *((GTE::RealDouble *)(dest)) = *((GTE::RealDouble *)(source)); (source) -= 2;(dest) ++;	\
	  *((GTE::Real *)(dest)) = 0;(dest) ++;														\
}
#endif

namespace GTE
{
	template <class T> class BaseVector
	{

	protected:

		/*
		* If [target] is not null, then use it as the storage array.
		*
		* If [permAttach] is true, then [target] cannot be null, as it will be used as the permanent
		* backing storage array. If [permAttach] is false, then this base vector can attach and
		* re-attach to various different backing storage arrays.
		*/
		void BaseVector::Init(Real *target, Bool permAttach)
		{
			data = baseData;

			if(target == nullptr && permAttach == true)
			{
				Debug::PrintWarning("BaseVector::Init -> 'permAttach' is true, but 'target' is null!");
				permAttach = false;
			}

			if(permAttach)
			{
				canDetach = false;
			}
			else
			{
				data = baseData;
				canDetach = true;
			}

			if(target != nullptr)
			{
				data = target;
				attached = true;
			}
			else
			{
				attached = false;
			}
		}

		Real * data;
		Real baseData[BaseVectorTraits<T>::VectorSize];
		Bool attached;
		Bool canDetach;

	public:

		/*
		* Default constructor
		*/
		BaseVector()
		{
			Init(nullptr, false);
			for(UInt32 i = 0; i < BaseVectorTraits<T>::VectorSize; i++)
			{
				data[i] = 0;
			}
		}

		/*
		* Constructor will alternate backing storage
		*/
		BaseVector(Bool permAttached, Real * target)
		{
			Init(target, true);
		}

		/*
		* Copy constructor
		*/
		BaseVector(const BaseVector<T>& baseVector)
		{
			Init(nullptr, false);

			for(UInt32 i = 0; i < BaseVectorTraits<T>::VectorSize; i++)
			{
				data[i] = baseVector.data[i];
			}
		}

		/*
		* Clean up
		*/
		virtual BaseVector::~BaseVector()
		{
			if(!attached)data = nullptr;
		}

		/*
		* Assignment operator.
		*/
		BaseVector& operator=(const BaseVector& source)
		{
			if(this == &source)return *this;

			attached = source.attached;
			canDetach = source.canDetach;

			for(UInt32 i = 0; i < BaseVectorTraits<T>::VectorSize; i++)
			{
				data[i] = source.data[i];
			}

			return *this;
		}

		/*
		* Set this base vector's data to be equal to [baseVector]
		*/
		void SetTo(const BaseVector& baseVector)
		{
			for(UInt32 i = 0; i < BaseVectorTraits<T>::VectorSize; i++)
			{
				data[i] = baseVector.data[i];
			}
		}

		/*
		* Copy this base vector's data into [baseVector]
		*/
		void Get(BaseVector& baseVector) const
		{
			memcpy(baseVector.data, data, sizeof(Real) * BaseVectorTraits<T>::VectorSize);
		}

		/*
		* Get a pointer the backing data storage
		*/
		Real * GetDataPtr()
		{
			return data;
		}

		/*
		* Attach the base vector to a new backing storage array [data]
		*/
		virtual void AttachTo(Real * data)
		{
			this->data = data;
			attached = true;
		}

		/*
		* Detach from the current backing storage array
		*/
		virtual void Detach()
		{
			if(canDetach)
			{
				this->data = baseData;
				attached = false;
			}
		}
	};
}

#endif
