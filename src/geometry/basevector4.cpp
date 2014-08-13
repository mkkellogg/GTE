#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector4.h"

void BaseVector4::Init(float x, float y, float z, float w)
{
    Set(x,y,z,w);
}

BaseVector4::BaseVector4() : data(new float[4]), baseData(data), attached(false)
{
    Init(0,0,0,0);
}

BaseVector4::BaseVector4(float x, float y, float z, float w) : data(new float[4]),  baseData(data), attached(false)
{
    Init(x,y,z,w);
}

BaseVector4::BaseVector4(const BaseVector4 * baseVector) : data(new float[4]), baseData(data), attached(false)
{
    Init(baseVector->data[0], baseVector->data[1], baseVector->data[2], baseVector->data[3]);
}

BaseVector4::~BaseVector4()
{   
    delete baseData;
    baseData = NULL;
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

float * BaseVector4::GetDataPtr()
{
    return data;
}

void BaseVector4::AttachTo(float * data)
{
    this->data = data;
    UpdateComponentPointers();
    attached = true;
}

void BaseVector4::Detach()
{
    this->data = baseData;
    UpdateComponentPointers();
    attached = false;
}





