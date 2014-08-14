#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "point3array.h"
#include "point3.h"
#include "ui/debug.h"

Point3Array::Point3Array(int count) : data(NULL), objects(NULL)
{
	bool initSuccess = Init(count);
	if(!initSuccess)return;
}

Point3Array::~Point3Array()
{
	Destroy();
}

void Point3Array::SetData(const float * data, bool includeW)
{

}

void Point3Array::Destroy()
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

bool Point3Array::Init(int count)
{
	data = new float[count * 4];
	if(data == NULL)
	{
		Debug::PrintError("Could not allocate data memory for Point3Array");
		return false;
	}

	objects = new Point3*[count];
	if(objects == NULL)
	{
		Debug::PrintError("Could not allocate objects memory for Point3Array");
		delete data;
		data = NULL;
		return false;
	}

	float *dataPtr = data;
	int index = 0;

	while(index < count)
	{
		Point3 * currentObject = new Point3(false, dataPtr);
		if(currentObject == NULL)
		{
			Debug::PrintError("Could not allocate Point3 for Point3Array");

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

Point3 * Point3Array::GetPoint(int index)
{
	return objects[index];
}

Point3 ** Point3Array::GetPoints()
{
	return objects;
}
