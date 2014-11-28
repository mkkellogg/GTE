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

BaseVector4Array::BaseVector4Array(BaseVector4Factory * factory) : maxCount(0), usedCount(0), data(NULL), objects(NULL), baseFactory(factory)
{

}

BaseVector4Array::~BaseVector4Array()
{
	Destroy();
}

void BaseVector4Array::Destroy()
{
	if(objects != NULL)baseFactory->DestroyArray(objects, maxCount);
	objects = NULL;

	SAFE_DELETE(data);
}

bool BaseVector4Array::Init(unsigned int maxCount)
{
	Destroy();

	this->maxCount = maxCount;
	this->usedCount = maxCount;

	data = new float[maxCount * 4];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for BaseVector4Array");
		return false;
	}

	objects = baseFactory->CreateArray(maxCount);

	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for BaseVector4Array");
		Destroy();
		return false;
	}

	float *dataPtr = data;

	unsigned int index = 0;
	while(index < maxCount)
	{
		BaseVector4 * currentObject = (BaseVector4*)baseFactory->CreatePermAttached(dataPtr);

		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate BaseVector4 for BaseVector4Array");
			Destroy();
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

unsigned int BaseVector4Array::GetMaxCount()
{
	return maxCount;
}

void BaseVector4Array::SetUsedCount(unsigned int usedCount)
{
	this->usedCount = usedCount;
}

unsigned int BaseVector4Array::GetUsedCount()
{
	return usedCount;
}

bool BaseVector4Array::CopyTo(BaseVector4Array * dest) const
{
	ASSERT(dest != NULL," BaseVector4Array::CopyTo -> Destination is NULL.",false);

	if(dest->GetMaxCount() != maxCount)
	{
		Debug::PrintError("BaseVector4Array::CopyTo -> Source count does not match destination count.");
		return false;
	}

	memcpy(dest->data, data, maxCount * sizeof(float) * 4);

	return true;
}

BaseVector4Array::Iterator BaseVector4Array::GetIterator(BaseVector4& targetVector)
{
	return Iterator(*this, targetVector);
}
