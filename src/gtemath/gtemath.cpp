#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "gtemath.h"

union IntFloatUnion
{
    int i;
    float f;
};

float GTEMath::SquareRoot(float n)
{
	return sqrt(n);
}

float GTEMath::QuickSquareRoot(float n)
{
	IntFloatUnion ifu;

	float threeHalfs = 1.5;
	float x2 = n * .5;

	ifu.f = n;
	ifu.i = 0x5f375a86 - (ifu.i >> 1);
	ifu.f = ifu.f * (threeHalfs - (x2 * ifu.f * ifu.f));

	return ifu.f * n;
}

float GTEMath::Cos(float n)
{
	return cos(n);
}

float GTEMath::Sin(float n)
{
	return sin(n);
}

float GTEMath::Abs(float n)
{
	return abs(n);
}

float GTEMath::Min(float a, float b)
{
	if(a < b)return a;
	else return b;
}

float GTEMath::Max(float a, float b)
{
	if(a > b)return a;
	else return b;
}

float GTEMath::Lerp(float a, float b, float t)
{
	return (b - a) * t + a;
}
