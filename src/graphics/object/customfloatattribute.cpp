#include "customfloatattribute.h"
#include "gtemath/gtemath.h"
#include "object/enginetypes.h"
#include "global/global.h"
#include "global/assert.h"
#include "util/engineutility.h"
#include "debug/gtedebug.h"
#include "engine.h"

#include <stdio.h>

namespace GTE
{
	CustomFloatAttribute::CustomFloatAttribute()
	{
		componentCount = 0;
		size = 0;
		data = nullptr;
	}

	Bool CustomFloatAttribute::Init(UInt32 size, UInt32 componentCount)
	{
		Destroy();

		this->componentCount = componentCount;
		this->size = size;
		data = new(std::nothrow) Real[componentCount * size];

		ASSERT(data != nullptr, "CustomFloatAttribute::Init -> Unable to allocate data.");

		return true;
	}

	void CustomFloatAttribute::Destroy()
	{
		if(data != nullptr)
		{
			delete data;
			data = nullptr;
		}
	}

	Real * CustomFloatAttribute::GetDataPtr()
	{
		return data;
	}

	UInt32 CustomFloatAttribute::GetSize() const
	{
		return size;
	}

	UInt32 CustomFloatAttribute::GetComponentCount() const
	{
		return componentCount;
	}
}