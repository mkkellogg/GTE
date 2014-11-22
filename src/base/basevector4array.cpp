#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector4array.h"
#include "basevector4.h"
#include "basevector4factory.h"
#include "global/global.h"
#include "ui/debug.h"

BaseVector4Array::BaseVector4Array(BaseVector4Factory * factory) : count(0), data(NULL), objects(NULL), baseFactory(factory)
{

}

BaseVector4Array::~BaseVector4Array()
{
	Destroy();
}

void BaseVector4Array::Destroy()
{
	if(objects != NULL)
	{
		for(unsigned int i=0; i < count; i++)
		{
			BaseVector4 * baseObj = objects[i];
			if(baseObj != NULL)
			{
				delete baseObj;
				objects[i] = NULL;
			}
		}
		delete objects;
		objects = NULL;
	}

	if(data != NULL)
	{
		delete data;
		data = NULL;
	}
}

bool BaseVector4Array::Init(unsigned int count)
{
	Destroy();

	this->count = count;

	data = new float[count * 4];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for BaseVector4Array");
		return false;
	}

	objects = baseFactory->CreateArray(count);

	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for BaseVector4Array");
		delete data;
		data = NULL;
		return false;
	}

	float *dataPtr = data;

	unsigned int index = 0;
	while(index < count)
	{
		BaseVector4 * currentObject = (BaseVector4*)baseFactory->CreatePermAttached(dataPtr);

		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate BaseVector4 for BaseVector4Array");

			for(unsigned int i=0; i< index; i++)delete objects[i];
			delete objects;
			objects = NULL;

			delete data;
			data = NULL;

			return false;
		}

		objects[index] = currentObject;
		currentObject->Set(0,0,0,0);

		dataPtr += 4;
		index++;
	}

	return true;
}

const float * BaseVector4Array::GetDataPtr() const
{
	return (const float *)data;
}

unsigned int BaseVector4Array::GetCount()
{
	return count;
}

bool BaseVector4Array::CopyTo(BaseVector4Array * dest) const
{
	ASSERT(dest != NULL," BaseVector4Array::CopyTo -> Destination is NULL.",false);

	if(dest->GetCount() != count)
	{
		Debug::PrintError("BaseVector4Array::CopyTo -> Source count does not match destination count.");
		return false;
	}

	memcpy(dest->data, data, count * sizeof(float) * 4);

	return true;
}

BaseVector4Array::Iterator BaseVector4Array::GetIterator(BaseVector4& targetVector)
{
	return Iterator(*this, targetVector);
}
