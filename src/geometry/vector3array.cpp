#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vector3array.h"
#include "vector3.h"
#include "ui/debug.h"

Vector3Array::Vector3Array(int count) : data(NULL), objects(NULL)
{
	bool initSuccess = Init(count);
	if(!initSuccess)return;
}

Vector3Array::~Vector3Array()
{
	Destroy();
}

void Vector3Array::SetData(const float * data, bool includeW)
{

}

void Vector3Array::Destroy()
{
	if(objects != NULL)
	{
		for(int i=0; i < count; i++)
		{
			delete objects[i];
			objects[i] = NULL;
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

bool Vector3Array::Init(int count)
{
	data = new float[count * 4];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for Vector3Array");
		return false;
	}

	objects = new Vector3*[count];
	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for Vector3Array");
		delete data;
		data = NULL;
		return false;
	}

	float *dataPtr = data;
	int index = 0;

	while(index < count)
	{
		Vector3 * currentObject = new Vector3(false, dataPtr);
		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate Vector3 for Vector3Array");

			for(int i=0; i< index; i++)delete objects[i];
			delete objects;
			objects = NULL;

			delete data;
			data = NULL;

			return false;
		}

		currentObject->Set(0,0,0);
		index++;
		dataPtr += 4;
	}

	return true;
}
