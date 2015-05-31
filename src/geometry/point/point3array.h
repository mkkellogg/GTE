#ifndef _GTE_POINT3ARRAY_H_
#define _GTE_POINT3ARRAY_H_

#include "base/basevector4array.h"

namespace GTE
{
	//forward declarations
	class Point3;

	class Point3Array : public BaseVector4Array
	{
	public:

		Point3Array();
		virtual ~Point3Array();

		void SetData(const Real * data, bool includeW);
		Point3 * GetPoint(int index);
		const Point3 * GetPointConst(int index) const;
		Point3 ** GetPoints();
	};

}

#endif
