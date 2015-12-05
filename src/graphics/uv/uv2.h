#ifndef _GTE_UV2_H_
#define _GTE_UV2_H_

#include "engine.h"
#include "base/basevector.h"
#include "base/basevectortraits.h"
#include "base/basevectorarray.h"

namespace GTE
{
	// forward declarations
	class UV2;

	template <> class BaseVectorTraits<UV2>
	{
		public:
		static const UInt32 VectorSize = 2;
	};

	class UV2 : public BaseVector<UV2>
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
		BaseVector<UV2>& operator= (const BaseVector<UV2>& source);

		void Set(Real u, Real v);

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};

	typedef BaseVectorArray<UV2> UV2Array;
}

#endif
