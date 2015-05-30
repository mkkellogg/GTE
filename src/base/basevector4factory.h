#ifndef _GTE_BASEVECTOR4_FACTORY_H_
#define _GTE_BASEVECTOR4_FACTORY_H_

namespace GTE
{
	// forward declarations
	class BaseVector4;

	class BaseVector4Factory
	{
	public:

		BaseVector4Factory();
		virtual ~BaseVector4Factory();

		virtual BaseVector4 * CreatePermAttached(float * target);
		virtual BaseVector4** CreateArray(int count);
		virtual void DestroyArray(BaseVector4 ** array, unsigned int size);
	};
}

#endif
