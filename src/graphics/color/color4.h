#ifndef _GTE_COLOR4_H_
#define _GTE_COLOR4_H_

#include "base/basevector.h"
#include "base/basevectortraits.h"
#include "base/basevectorarray.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class Color4;

	template <> class BaseVectorTraits<Color4>
	{
		public:
		static const UInt32 VectorSize = 4;
	};

	class Color4 : public BaseVector<Color4>
	{
	protected:

	public:

		Real &r;
		Real &g;
		Real &b;
		Real &a;

		Color4();
		Color4(Bool permAttached, Real * target);
		Color4(Real r, Real g, Real b, Real a);
		Color4(const Color4& point);
		~Color4();

		Color4& operator= (const Color4& source);
		BaseVector<Color4>& operator= (const BaseVector<Color4>& source);
		void Set(Real r, Real g, Real b, Real a);
		Real MaxComponentMagnitude();

		void Add(const Color4& c);
		static void Add(const Color4& c1, const Color4& c2, Color4& result);
		void Multiply(const Color4& c);
		static void Multiply(const Color4& c1, const Color4& c2, Color4& results);
		
		void Lerp(const Color4& c1, const Color4& c2, Real t);
		static void Lerp(const Color4& c1, const Color4& c2, Color4& result, Real t);

		void Scale(Real magnitude);
		void Normalize();

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};

	typedef BaseVectorArray<Color4> Color4Array;
}

#endif
