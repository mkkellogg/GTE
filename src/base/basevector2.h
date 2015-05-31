#ifndef _GTE_BASEVECTOR2_H_
#define _GTE_BASEVECTOR2_H_

#include "object/enginetypes.h"
#include "global/global.h"

namespace GTE
{
	class BaseVector2
	{
	protected:

		void Init(Real *target, bool permAttach);

		Real * data;
		Real baseData[2];
		bool attached;
		bool canDetach;

	public:

		BaseVector2();
		BaseVector2(bool permAttached, Real * target);
		BaseVector2(Real x, Real y);
		BaseVector2(const BaseVector2& baseVector);
		BaseVector2(const Real * copyData);
		virtual ~BaseVector2();

		BaseVector2 & operator= (const BaseVector2 & source);
		Real * GetDataPtr() const;
		void Set(Real x, Real y);
		void Get(BaseVector2& baseVector);

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};
}

#endif
