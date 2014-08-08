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
