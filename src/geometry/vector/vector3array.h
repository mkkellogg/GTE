#ifndef _GTE_VECTOR3ARRAY_H_
#define _GTE_VECTOR3ARRAY_H_

#include "base/basevector4array.h"
#include "vector3.h"

namespace GTE
{
	//forward declarations
	class Vector3;

	class Vector3Array : public BaseVector4Array<Vector3>
	{
	public:

		Vector3Array();
		~Vector3Array() override;

		void SetData(const Real * data, Bool includeW);
		Vector3 * GetVector(Int32 index);
		const Vector3 * GetVectorConst(Int32 index) const;
		Vector3 ** GetVectors();
	};
}
#endif
