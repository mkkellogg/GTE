#include "engine.h"
#include "customfloatattributebuffer.h"
#include "gtemath/gtemath.h"
#include "global/global.h"
#include "global/assert.h"
#include "util/engineutility.h"
#include "debug/gtedebug.h"


namespace GTE
{
	CustomFloatAttributeBuffer::CustomFloatAttributeBuffer()
	{
		componentCount = 0;
		size = 0;
		data = nullptr;
	}

	CustomFloatAttributeBuffer::~CustomFloatAttributeBuffer()
	{
		Destroy();
	}

	Bool CustomFloatAttributeBuffer::Init(UInt32 size, UInt32 componentCount)
	{
		Destroy();

		this->componentCount = componentCount;
		this->size = size;

		data = new(std::nothrow) Real[componentCount * size];
		ASSERT(data != nullptr, "CustomFloatAttributeBuffer::Init -> Unable to allocate data.");

		return true;
	}

	void CustomFloatAttributeBuffer::Destroy()
	{
		if(data != nullptr)
		{
			delete data;
			data = nullptr;
		}
	}

	void CustomFloatAttributeBuffer::SetAttributeID(AttributeID id)
	{
		this->attributeID = id;
	}

	AttributeID CustomFloatAttributeBuffer::GetAttributeID()
	{
		return attributeID;
	}

	Real * CustomFloatAttributeBuffer::GetDataPtr()
	{
		return data;
	}

	UInt32 CustomFloatAttributeBuffer::GetSize() const
	{
		return size;
	}

	UInt32 CustomFloatAttributeBuffer::GetComponentCount() const
	{
		return componentCount;
	}
}
