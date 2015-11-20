#ifndef _GTE_BASEVECTOR2ARRAY_H_
#define _GTE_BASEVECTOR2ARRAY_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class BaseVector2;
	class BaseVector2Factory;

	class BaseVector2Array
	{
	protected:
		
		Int32 count;
		Real * data;
		BaseVector2 ** objects;
		BaseVector2Factory * baseFactory;

		void Destroy();

	public:

		BaseVector2Array(BaseVector2Factory * factory);
		virtual ~BaseVector2Array();

		const Real * GetDataPtr() const;
		Bool Init(Int32 count);
	};
}

#endif
