#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector2array.h"
#include "basevector2.h"
#include "basevector2factory.h"
#include "global/global.h"
#include "debug/gtedebug.h"

BaseVector2Array::BaseVector2Array(BaseVector2Factory * factory) : count(0), data(NULL), objects(NULL), baseFactory(factory)
{

}

BaseVector2Array::~BaseVector2Array()
{
	Destroy();
}

void BaseVector2Array::Destroy()
{
	if(objects!=NULL)baseFactory->DestroyArray(objects, count);
	objects = NULL;

	SAFE_DELETE(data);
}

bool BaseVector2Array::Init(int count)
{
	Destroy();

	this->count = count;

	data = new float[count * 2];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for BaseVector2Array");
		return false;
	}

	objects = baseFactory->CreateArray(count);

	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for BaseVector2Array");
		Destroy();
		return false;
	}

	float *dataPtr = data;

	int index = 0;
	while(index < count)
	{
		BaseVector2 * currentObject = (BaseVector2*)baseFactory->CreatePermAttached(dataPtr);

		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate BaseVector2 for BaseVector2Array");
			Destroy();
			return false;
		}

		objects[index] = currentObject;
		currentObject->Set(0,0);

		dataPtr += 2;
		index++;
	}

	return true;
}

const float * BaseVector2Array::GetDataPtr() const
{
	return (const float *)data;
}



