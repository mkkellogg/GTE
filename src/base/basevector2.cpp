#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector2.h"

void BaseVector2::Init(float x, float y, float *target, bool permAttach)
{
	data = baseData = NULL;

	if(permAttach)
	{
		canDetach = false;
	}
	else
	{
		data = baseData = new float[4];
		canDetach = true;
	}

	if(target != NULL)
	{
		data = target;
		attached = true;
	}
	else
	{
		attached = false;
	}
    Set(x,y);
}

BaseVector2::BaseVector2()
{
    Init(0,0, NULL, false);
}

BaseVector2::BaseVector2(bool permAttached, float * target)
{
	Init(0,0,target, true);
}

BaseVector2::BaseVector2(float x, float y)
{
    Init(x,y, NULL, false);
}

BaseVector2::BaseVector2(const BaseVector2& baseVector)
{
    Init(baseVector.data[0], baseVector.data[1], NULL, false);
}

BaseVector2::~BaseVector2()
{   
	if(baseData != NULL)
	{
		delete baseData;
		baseData = NULL;
	}

    if(!attached)data = NULL;
}

BaseVector2 & BaseVector2::operator= (const BaseVector2 & source)
{
	if(this == &source)return * this;

	attached = source.attached;
	canDetach = source.canDetach;

	data[0] = source.data[0];
	data[1] = source.data[1];

	return *this;
}

void BaseVector2::Set(float x, float y)
{
    data[0] = x;
    data[1] = y;
}

void BaseVector2::Get(BaseVector2& baseVector)
{
   memcpy(baseVector.data, data, sizeof(float) * 2);
}

float * BaseVector2::GetDataPtr() const
{
    return data;
}

void BaseVector2::AttachTo(float * data)
{
    this->data = data;
    attached = true;
}

void BaseVector2::Detach()
{
	if(canDetach)
	{
		this->data = baseData;
		attached = false;
	}
}





