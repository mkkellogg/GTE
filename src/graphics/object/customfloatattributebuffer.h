
/*
* class: Particle
*
* author: Mark Kellogg
*
* Descriptor for custom floating point vertex attribute data.
*/

#ifndef _GTE_CUSTOM_FLOAT_ATTRIBUTE_BUFFER_H_
#define _GTE_CUSTOM_FLOAT_ATTRIBUTE_BUFFER_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "gtemath/gtemath.h"
#include <vector>
#include <string>

namespace GTE
{
	class CustomFloatAttributeBuffer
	{
		friend class SubMesh3D;

		Real * data;
		UInt32 componentCount;
		UInt32 size;

		CustomFloatAttributeBuffer();
		Bool Init(UInt32 size, UInt32 componentCount);
		void Destroy();

		public:
		
		Real * GetDataPtr();
		UInt32 GetSize() const;
		UInt32 GetComponentCount() const;
	};
}

#endif 