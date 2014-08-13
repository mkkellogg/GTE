#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector4.h"

void BaseVector4::Init(float x, float y, float z, float w, float *target, bool permAttach)
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
    Set(x,y,z,w);
}

BaseVector4::BaseVector4()
{
    Init(0,0,0,0, NULL, false);
}

BaseVector4::BaseVector4(bool permAttached, float * target)
{
	Init(0,0,0,0,target, true);
}

BaseVector4::BaseVector4(float x, float y, float z, float w)
{
    Init(x,y,z,w, NULL, false);
}

BaseVector4::BaseVector4(const BaseVector4 * baseVector)
{
    Init(baseVector->data[0], baseVector->data[1], baseVector->data[2], baseVector->data[3], NULL, false);
}

BaseVector4::~BaseVector4()
{   
	if(baseData != NULL)
	{
		delete baseData;
		baseData = NULL;
	}

    if(!attached)data = NULL;
}

BaseVector4 & BaseVector4::operator= (const BaseVector4 & source)
{
    if(this == &source)return *this;
    memcpy((void*)source.data, (void*)data, sizeof(float) * 4); 
    return *this;
}

void BaseVector4::Set(float x, float y, float z, float w)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

void BaseVector4::Set(float x, float y, float z)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

void BaseVector4::Get(BaseVector4 * baseVector)
{
   memcpy(baseVector->data, data, sizeof(float) * 4);
}

float * BaseVector4::GetDataPtr() const
{
    return data;
}

void BaseVector4::AttachTo(float * data)
{
    this->data = data;
    attached = true;
}

void BaseVector4::Detach()
{
	if(canDetach)
	{
		this->data = baseData;
		attached = false;
	}
}





