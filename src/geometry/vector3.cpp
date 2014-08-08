#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "vector3.h"


Vector3::Vector3() : BaseVector3()
{
   
}

Vector3::Vector3(float x, float y, float z) : BaseVector3(x,y,z)
{
    
}

Vector3::Vector3(Vector3 * vector) : BaseVector3(vector)
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

public void scale(float magnitude)
{
X *= magnitude;
Y *= magnitude;
Z *= magnitude;
}
public void setCopy(float[] components)
{
set(components[0],components[1],components[2]);
}
public void normalize()
{
float magnitude = (float)(Math.sqrt(X*X + Y*Y + Z*Z));
if( magnitude != 0f)
{
X = X/magnitude;
Y = Y/magnitude;
Z= Z/magnitude;
}
}
public float magnitude()
{
return (float)Math.sqrt(X*X + Y*Y + Z*Z);
}
public void invert()
{
X = -X;
Y = -Y;
Z = -Z;
}
public static Vector3 cross(Vector3 a, Vector3 b)
{
float x,y,z;
x = (a.Y*b.Z) - (b.Y*a.Z);
y = (b.X*a.Z) - (a.X*b.Z);
z = (a.X*b.Y) - (b.X*a.Y);	
Vector3 vec = new Vector3(x,y,z);
return vec;
}	
public static void cross(Vector3 a, Vector3 b, Vector3 results)
{
float x,y,z;
x = (a.Y*b.Z) - (b.Y*a.Z);
y = (b.X*a.Z) - (a.X*b.Z);
z = (a.X*b.Y) - (b.X*a.Y);	
results.set(x,y,z);
}

//static
static void calcNormal(Vector3 a, Vector3 b, Vector3 result)
{
cross(a,b,result);
result.normalize();
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

//static
float dot(Vector3 * a, Vector3 * b)
{
    float x = a->x * b->x;
    float y = a->y * b->y;
    float z = a->z * b->z;
    return x+y+z;
}

// static
float Vector3::AngleBetween(Vector3 * a, Vector3 * b, Vector3 * refRightNormal)
{

}


