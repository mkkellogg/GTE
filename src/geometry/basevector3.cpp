#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector3.h"

void BaseVector3::Init(float x, float y, float z, float w)
{
    Set(x,y,z,w);
}

BaseVector3::BaseVector3() : data(new float[4]), baseData(data), attached(false), x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
    Init(0,0,0,0);
}

BaseVector3::BaseVector3(float x, float y, float z, float w) : data(new float[4]),  baseData(data), attached(false), x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
    Init(x,y,z,w);
}

BaseVector3::BaseVector3(BaseVector3 * baseVector) : data(new float[4]), baseData(data), attached(false), x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
    Init(baseVector->x, baseVector->y, baseVector->z, baseVector->w);
}

BaseVector3::BaseVector3(float * copyData) : data(new float[4]), baseData(data), attached(false), x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
    Init(copyData[0],copyData[1],copyData[2], copyData[3]);
}

BaseVector3::~BaseVector3()
{   
    delete baseData;
    baseData = NULL;

    if(!attached)data = NULL;
}

BaseVector3 & BaseVector3::operator= (const BaseVector3 & source)
{
    if(this == &source)return *this;
    memcpy((void*)source.data, (void*)data, sizeof(float) * 4); 
    return *this;
}

void BaseVector3::Set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;    
}

void BaseVector3::Set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void BaseVector3::Get(BaseVector3 * baseVector)
{
   memcpy(data, baseVector->data, sizeof(float) * 4);
}

float * BaseVector3::GetDataPtr()
{
    return data;
}

void BaseVector3::AttachTo(float * data)
{
    this->data = data;
    UpdateComponentPointers();
    attached = true;
}

void BaseVector3::Detach()
{
    this->data = baseData;
    UpdateComponentPointers();
    attached = false;
}

void BaseVector3::UpdateComponentPointers()
{
    float ** rPtr;
    rPtr = (float **)&x;
    *rPtr = data;
    rPtr = (float **)&y;
    *rPtr = data+1;
    rPtr = (float **)&z;
    *rPtr = data+2;
}



