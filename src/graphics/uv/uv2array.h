#ifndef _GTE_UV2_ARRAY_H_
#define _GTE_UV2_ARRAY_H_

#include "uv2.h"
#include "base/basevector2.h"
#include "base/basevector2array.h"

namespace GTE
{
	class UV2Array : public BaseVector2Array
	{
	public:

		UV2Array();
		virtual ~UV2Array();

		void SetData(const Real * data);
		UV2 * GetCoordinate(int index);
		UV2 ** GetCoordinates();
	};
}

#endif
