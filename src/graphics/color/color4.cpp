#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "color4.h"

Color4::Color4() : BaseVector4(), r(data[0]), g(data[1]), b(data[2]), a(data[3])
{

}

Color4::Color4(bool permAttached, float * target) : BaseVector4(permAttached, target), r(data[0]), g(data[1]), b(data[2]), a(data[3])
{


}

Color4::Color4(float r, float g, float b, float a) : BaseVector4(r,g,b,a), r(data[0]), g(data[1]), b(data[2]), a(data[3])
{
   
}

Color4::Color4(const Color4 * color) : BaseVector4(color), r(data[0]), g(data[1]), b(data[2]), a(data[3])
{
   
}

Color4::~Color4()
{
    
}

void Color4::AttachTo(float * data)
{
	BaseVector4::AttachTo(data);
	UpdateComponentPointers();
}


void Color4::Detach()
{
	BaseVector4::Detach();
	UpdateComponentPointers();
}

void Color4::UpdateComponentPointers()
{
    float ** rPtr;
    rPtr = (float **)&r;
    *rPtr = data;
    rPtr = (float **)&g;
    *rPtr = data+1;
    rPtr = (float **)&b;
    *rPtr = data+2;
    rPtr = (float **)&a;
    *rPtr = data+2;
}

