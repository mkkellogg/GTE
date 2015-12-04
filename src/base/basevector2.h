#ifndef _GTE_BASEVECTOR2_H_
#define _GTE_BASEVECTOR2_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	class BaseVector2
	{
	protected:

		void Init(Real *target, Bool permAttach);

		Real * data;
		Real baseData[2];
		Bool attached;
		Bool canDetach;

	public:

		BaseVector2();
		BaseVector2(Bool permAttached, Real * target);
		BaseVector2(Real x, Real y);
		BaseVector2(const BaseVector2& baseVector);
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
