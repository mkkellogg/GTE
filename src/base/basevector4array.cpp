#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector4array.h"
#include "basevector4.h"
#include "basevector4factory.h"
#include "geometry/point3factory.h"
#include "ui/debug.h"

BaseVector4Array::BaseVector4Array(int count, BaseVector4Factory * factory) : count(count), data(NULL), objects(NULL), baseFactory(factory)
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
		for(int i=0; i < count; i++)
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

bool BaseVector4Array::Init()
{
	data = new float[count * 4];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for BaseVector4Array");
		return false;
	}

	Debug::PrintMessage(">> Allocated data memory for BaseVector4Array!");

	baseFactory->CreateArray(count, &objects);

	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for BaseVector4Array");
		delete data;
		data = NULL;
		return false;
	}

	Debug::PrintMessage(">> Allocated objects memory for BaseVector4Array!");

	float *dataPtr = data;


	char strData[64];

	int index = 0;
	while(index < count)
	{
		BaseVector4 * currentObject = (BaseVector4*)baseFactory->CreatePermAttached(dataPtr);

		sprintf(strData, ">> CreatePermAttached() success! %d", index);
		Debug::PrintMessage(strData);

		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate BaseVector4 for BaseVector4Array");

			for(int i=0; i< index; i++)delete objects[i];
			delete objects;
			objects = NULL;

			delete data;
			data = NULL;

			return false;
		}

		currentObject->Set(0,0,0);

		dataPtr += 4;
		index++;
	}

	Debug::PrintMessage("init complete!");

	return true;
}


