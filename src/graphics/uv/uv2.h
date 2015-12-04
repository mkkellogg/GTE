#ifndef _GTE_UV2_H_
#define _GTE_UV2_H_

#include "engine.h"
#include "base/basevector2.h"
#include "base/basevector2array.h"

namespace GTE
{
	class UV2 : public BaseVector2
	{
	protected:

	public:

		Real &u;
		Real &v;

		UV2();
		UV2(Bool permAttached, Real * target);
		UV2(Real u, Real v);
		UV2(const UV2& uv);
		~UV2();

		UV2 & operator= (const UV2 & source);
		BaseVector2& operator= (const BaseVector2& source);

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};

	typedef BaseVector2Array<UV2> UV2Array;
}

#endif
