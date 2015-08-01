/*
 * class: BaseVector4
 *
 * author: Mark Kellogg
 *
 * Forms the basis of several vertex attribute classes (Point3, Vector3, Color4, ...) and contains
 * functionality that is shared by all of them.
 */

#ifndef _GTE_BASEVECTOR4_H_
#define _GTE_BASEVECTOR4_H_

#include "object/enginetypes.h"
#include "global/global.h"

#define BaseVector4_QuickCopy(source, dest)        \
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=3;					       		   	   \
  (dest)-=3;							   		   \
}

#define BaseVector4_QuickCopy_IncDest(source, dest)\
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=3;					       		   	   \
  (dest)++;							   		   	   \
}

#define BaseVector4_QuickCopy_ZeroW(source, dest)  \
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=2;					       		   	   \
  (dest)++;					   		   		       \
  *(dest) = 0;                      	   		   \
  (dest)-=3;						   		   	   \
}

#define BaseVector4_QuickCopy_ZeroW_IncDest(source, dest)\
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=2;					       		   	   \
  (dest)++;					   		   		       \
  *(dest) = 0;                      	   		   \
  (dest)++;					   		   		       \
}

namespace GTE
{
	class BaseVector4
	{
		friend class BaseVector4Array;

	protected:

		void Init(Real *target, Bool permAttach);

		Real * data;
		Real baseData[4];
		Bool attached;
		Bool canDetach;

	public:

		BaseVector4();
		BaseVector4(Bool permAttached, Real * target);
		BaseVector4(Real x, Real y, Real z, Real w);
		BaseVector4(const BaseVector4& baseVector);
		BaseVector4(const Real * copyData);
		virtual ~BaseVector4();

		BaseVector4& operator=(const BaseVector4& source);

		Real * GetDataPtr();
		void Set(Real x, Real y, Real z, Real w);
		void SetTo(const BaseVector4& baseVector);
		void Get(BaseVector4& baseVector) const;

		void AttachTo(Real * data);
		void Detach();
	};
}

#endif
