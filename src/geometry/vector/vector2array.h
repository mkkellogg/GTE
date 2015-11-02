#ifndef _GTE_UV2_ARRAY_H_
#define _GTE_UV2_ARRAY_H_

#include "vector2.h"
#include "base/basevector2.h"
#include "base/basevector2array.h"

namespace GTE
{
	class Vector2Array : public BaseVector2Array
	{
		public:

		Vector2Array();
		virtual ~Vector2Array();

		void SetData(const Real * data);
		Vector2 * GetCoordinate(Int32 index);
		Vector2 ** GetCoordinates();
	};
}

#endif
