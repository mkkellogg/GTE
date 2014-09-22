#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "uv2.h"

UV2::UV2() : BaseVector2(), u(data[0]), v(data[1])
{

}

UV2::UV2(bool permAttached, float * target) : BaseVector2(permAttached, target),  u(data[0]), v(data[1])
{


}

UV2::UV2(float u, float v) : BaseVector2(u,v), u(data[0]), v(data[1])
{
   
}

UV2::UV2(const UV2 * uv) : BaseVector2(uv), u(data[0]), v(data[1])
{
   
}

UV2::~UV2()
{
    
}

/*
 * Assignment operator
 */
UV2 & UV2::operator= (const UV2 & source)
{
    if(this == &source)return *this;
    memcpy((void*)source.data, (void*)data, sizeof(float) * 2);
    memcpy((void*)source.baseData, (void*)baseData, sizeof(float) * 2);
    return *this;
}

void UV2::AttachTo(float * data)
{
	BaseVector2::AttachTo(data);
	UpdateComponentPointers();
}

void UV2::Detach()
{
	BaseVector2::Detach();
	UpdateComponentPointers();
}

void UV2::UpdateComponentPointers()
{
    float ** rPtr;
    rPtr = (float **)&u;
    *rPtr = data;
    rPtr = (float **)&v;
    *rPtr = data+1;
}


