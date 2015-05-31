#ifndef _GTE_COLOR4_ARRAY_H_
#define _GTE_COLOR4_ARRAY_H_

#include "base/basevector4array.h"

namespace GTE
{
	//forward declarations
	class Color4;

	class Color4Array : public BaseVector4Array
	{
	public:

		Color4Array();
		virtual ~Color4Array();

		void SetData(const Real * data, Bool includeW);
		Color4 * GetColor(Int32 index);
		Color4 ** GetColors();
	};
}

#endif
