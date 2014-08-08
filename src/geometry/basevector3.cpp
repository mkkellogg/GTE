
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector3.h"

void BaseVector3::Init(float x, float y, float z)
{
    Set(x,y,z);
    w = 1;
}

BaseVector3::BaseVector3() : data(new float[4]), deleteData(true), x(data[0]), y(data[1]), z(data[2]), w(data[4])
{
    Init(0,0,0);
}

BaseVector3::BaseVector3(float x, float y, float z) : data(new float[4]), deleteData(true), x(data[0]), y(data[1]), z(data[2]), w(data[4])
{
    Init(x,y,z);
}

BaseVector3::BaseVector3(BaseVector3 * baseVector) : data(new float[4]), deleteData(true), x(data[0]), y(data[1]), z(data[2]), w(data[4])
{
    Init(baseVector->x, baseVector->y, baseVector->z);
}

BaseVector3::~BaseVector3()
{
    if(deleteData)
    {
        delete data;
        deleteData = false;
        data = NULL;
    }
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


