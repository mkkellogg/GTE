
/*
* class: Particle
*
* author: Mark Kellogg
*
* Descriptor for custom floating point vertex attribute data.
*/

#ifndef _GTE_CUSTOM_FLOAT_ATTRIBUTE_BUFFER_H_
#define _GTE_CUSTOM_FLOAT_ATTRIBUTE_BUFFER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "gtemath/gtemath.h"
#include "graphics/materialvardirectory.h"

namespace GTE
{
	class CustomFloatAttributeBuffer
	{
		friend class SubMesh3D;

		Real * data;
		UInt32 componentCount;
		UInt32 size;
		AttributeID attributeID;

		
		Bool Init(UInt32 size, UInt32 componentCount);
		void Destroy();

		public:
		
		CustomFloatAttributeBuffer();
		virtual ~CustomFloatAttributeBuffer();

		void SetAttributeID(AttributeID id);
		AttributeID GetAttributeID();

		Real * GetDataPtr();
		UInt32 GetSize() const;
		UInt32 GetComponentCount() const;
	};
}

#endif 
