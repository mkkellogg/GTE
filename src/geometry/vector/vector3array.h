#ifndef _GTE_VECTOR3ARRAY_H_
#define _GTE_VECTOR3ARRAY_H_

#include "base/basevector4array.h"

namespace GTE
{
	//forward declarations
	class Vector3;

	class Vector3Array : public BaseVector4Array
	{
	public:

		Vector3Array();
		virtual ~Vector3Array();

		void SetData(const float * data, bool includeW);
		Vector3 * GetVector(int index);
		const Vector3 * GetVectorConst(int index) const;
		Vector3 ** GetVectors();
	};
}
#endif
