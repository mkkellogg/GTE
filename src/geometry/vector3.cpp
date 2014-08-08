#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "vector3.h"
#include "gtemath/gtemath.h"

Vector3::Vector3() : BaseVector3()
{
   
}

Vector3::Vector3(float x, float y, float z) : BaseVector3(x,y,z,0)
{
    
}

Vector3::Vector3(Vector3 * vector) : BaseVector3(vector)
{
    
}


Vector3::Vector3(float * data) : BaseVector3(data)
{

}

Vector3::~Vector3()
{
   
}

void Vector3::Add(Vector3 * v)
{
    x += v->x;
    y += v->y;
    z += v->z;
}

void Vector3::Add(Vector3 * v1, Vector3 * v2, Vector3 * result)
{
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;
}

void Vector3::Subtract(Vector3 * v1, Vector3 * v2, Vector3 * result)
{
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;
}

void Vector3::Scale(float magnitude)
{
    x *= magnitude;
    y *= magnitude;
    z *= magnitude;
}

void Vector3::Normalize()
{
    float magnitude = Magnitude();
    if(magnitude != 0)
    {
        Scale(1/magnitude);
    }
}

void Vector3::QuickNormlize()
{
    float magnitude = QuickMagnitude();
    if(magnitude != 0)
    {
        Scale(1/magnitude);
    }
}

float Vector3::Magnitude()
{
    return (float)sqrt(x*x + y*y + z*z);
}

float Vector3::QuickMagnitude()
{
    return GTEMath::QuickSquareRoot(x*x + y*y + z*z);
}

void Vector3::Invert()
{
    x = -x;
    y = -y;
    z = -z;
}

void Vector3::Cross(Vector3 * a, Vector3 * b, Vector3 * results)
{
    float x,y,z;
    x = (a->y*b->z) - (b->y*a->z);
    y = (b->x*a->z) - (a->x*b->z);
    z = (a->x*b->y) - (b->x*a->y);	
    results->Set(x,y,z);
}

void Vector3::CalcNormal(Vector3 * a, Vector3 * b, Vector3 * result)
{
    Cross(a,b,result);
    result->Normalize();
}


float Vector3::Dot(Vector3 * a, Vector3 * b)
{
    float x = a->x * b->x;
    float y = a->y * b->y;
    float z = a->z * b->z;
    return x+y+z;
}

// static
float Vector3::AngleBetween(Vector3 * a, Vector3 * b, Vector3 * refRightNormal)
{
    return 0 ;
}

/*
public static void avgVectors(Vector3 a, Vector3 b, Vector3 avg)
{
avg.X = (a.X+b.X)/2f;
avg.Y = (a.Y+b.Y)/2f;
avg.Z = (a.Z+b.Z)/2f;
}
public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 avg)
{
avg.X = (a.X+b.X+c.X)/3f;
avg.Y = (a.Y+b.Y+c.Y)/3f;
avg.Z = (a.Z+b.Z+c.Z)/3f;
}*/

/*
public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 avg)
{
avg.X = (a.X+b.X+c.X+d.X)/4f;
avg.Y = (a.Y+b.Y+c.Y+d.Y)/4f;
avg.Z = (a.Z+b.Z+c.Z+d.Z)/4f;
}
public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float[] weights, Vector3 avg)
{
float totalWeight =0f;
for(int i=0; i <4; i++)totalWeight += weights[i];
avg.X = ((weights[0]*a.X)+(weights[1]*b.X)+(weights[2]*c.X)+(weights[3]*d.X))/totalWeight;
avg.Y = ((weights[0]*a.Y)+(weights[1]*b.Y)+(weights[2]*c.Y)+(weights[3]*d.Y))/totalWeight;
avg.Z = ((weights[0]*a.Z)+(weights[1]*b.Z)+(weights[2]*c.Z)+(weights[3]*d.Z))/totalWeight;
}*/

/*
public static Vector3 createNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
Vector3 A = new Vector3(x1,y1,z1, x2,y2,z2);
Vector3 B = new Vector3(x1,y1,z1, x3,y3,z3);
Vector3 vec= cross(A,B);
vec.normalize();
return vec;
}*/




