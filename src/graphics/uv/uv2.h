#ifndef _GTE_UV2_H_
#define _GTE_UV2_H_

#include "uv2.h"
#include "base/basevector2.h"

namespace GTE
{
	class UV2 : public BaseVector2
	{
	protected:

	public:

		Real &u;
		Real &v;

		UV2();
		UV2(bool permAttached, Real * target);
		UV2(Real u, Real v);
		UV2(const UV2& uv);
		UV2(const Real * data);
		~UV2();

		void AttachTo(Real * data);
		void Detach();

		UV2 & operator= (const UV2 & source);
		BaseVector2& operator= (const BaseVector2& source);
	};
}

#endif
