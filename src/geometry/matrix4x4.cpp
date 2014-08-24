/********************************************* 
*
* Matrix math utilities. These methods operate on OpenGL ES format matrices and 
* vectors stored in float arrays. Matrices are 4 x 4 column-vector matrices 
* stored in column-major order:
*
* m[offset +  0] m[offset +  4] m[offset +  8] m[offset + 12]
* m[offset +  1] m[offset +  5] m[offset +  9] m[offset + 13]
* m[offset +  2] m[offset +  6] m[offset + 10] m[offset + 14]
* m[offset +  3] m[offset +  7] m[offset + 11] m[offset + 15]
* 
* Vectors are 4 row x 1 column column-vectors stored in order:
* v[offset + 0]
* v[offset + 1]
* v[offset + 2]
* v[offset + 3]
*
***********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "ui/debug.h"
#include "matrix4x4.h"
#include "point/point3.h"
#include "vector/vector3.h"

#define I(_i, _j) ((_j)+ DIM_SIZE*(_i))
#define PI 3.14159265

Matrix4x4::Matrix4x4()
{
    Init();
    SetIdentity();
}

Matrix4x4::Matrix4x4(float * data)
{
    Init();
    memcpy(this->data, data, sizeof(float) * DATA_SIZE);
}

Matrix4x4::~Matrix4x4()
{
    
}

int Matrix4x4::GetDataSize() const
{
	return DATA_SIZE;
}

void Matrix4x4::SetTo(const Matrix4x4 * src)
{
	if(this == src)return;
	SetTo(src->data);
}

void Matrix4x4::SetTo(const float * srcData)
{
	if(srcData == NULL)
	{
		Debug::PrintError("Matrix::Set() -> srcData is NULL");
		return;
	}

	memcpy((void*)this->data,(void*)srcData, sizeof(float) * DATA_SIZE);
}

Matrix4x4 & Matrix4x4::operator= (const Matrix4x4 & source)
{
    if(this == &source)return *this;
    SetTo(&source);
    return *this;
}

void Matrix4x4::Init()
{
    
}

void Matrix4x4::Transform(const Vector3 * vector, Vector3 * out) const
{
    MultiplyMV(this->data, vector->GetDataPtr(), out->GetDataPtr());
}

void Matrix4x4::Transform(const Point3 * point, Point3 * out) const
{
    MultiplyMV(this->data, point->GetDataPtr(), out->GetDataPtr());
}

void Matrix4x4::Transform(Vector3 * vector) const
{
	float temp[DIM_SIZE];
	MultiplyMV(this->data, vector->GetDataPtr(), temp);
	memcpy(vector->GetDataPtr(), temp, sizeof(float) * DIM_SIZE);
}

void Matrix4x4::Transform(Point3 * point) const
{
	float temp[DIM_SIZE];
	MultiplyMV(this->data, point->GetDataPtr(), temp);
	memcpy(point->GetDataPtr(), temp, sizeof(float) * DIM_SIZE);
}

void Matrix4x4::Multiply(const Matrix4x4 * matrix)
{
    float temp[DATA_SIZE];
    MultiplyMM(this->data, matrix->data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::LeftMultiply(const Matrix4x4 * matrix)
{
    float temp[DATA_SIZE];
    MultiplyMM(matrix->data, this->data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}


void Matrix4x4::Multiply(const Matrix4x4 * matrix, Matrix4x4 * out) const
{
    MultiplyMM(matrix->data, this->data, out->data);
}

void Matrix4x4::Multiply(const Matrix4x4 * lhs, const Matrix4x4 *rhs, Matrix4x4 * out)
{
    MultiplyMM(lhs->data, rhs->data, out->data);
}

void Matrix4x4::MultiplyMV(const float * lhsMat, const float * rhsVec, float * out)
{
    Mx4transform(rhsVec[0], rhsVec[1], rhsVec[2], rhsVec[3], lhsMat, out);
}

void Matrix4x4::Mx4transform(float x, float y, float z, float w, const float* pM, float* pDest) 
{
    pDest[0] = pM[0 + 4 * 0] * x + pM[0 + 4 * 1] * y + pM[0 + 4 * 2] * z + pM[0 + 4 * 3] * w;
    pDest[1] = pM[1 + 4 * 0] * x + pM[1 + 4 * 1] * y + pM[1 + 4 * 2] * z + pM[1 + 4 * 3] * w;
    pDest[2] = pM[2 + 4 * 0] * x + pM[2 + 4 * 1] * y + pM[2 + 4 * 2] * z + pM[2 + 4 * 3] * w;
    pDest[3] = pM[3 + 4 * 0] * x + pM[3 + 4 * 1] * y + pM[3 + 4 * 2] * z + pM[3 + 4 * 3] * w;
}

/*********************************************************
*
* Multiply two 4x4 matrices together and store the result in a third 4x4 matrix. 
* In matrix notation: out = lhs x rhs. Due to the way matrix multiplication works, 
* the 'out' matrix will have the same effect as first multiplying by the 'rhs' matrix, 
* then multiplying by the 'lhs' matrix. 
* 
* Parameters:
* 'out' The float array that holds the result.
* 'lhs' The float array that holds the left-hand-side 4x4 matrix.
* 'rhs' The float array that holds the right-hand-side 4x4 matrix.
*
*********************************************************/

void Matrix4x4::MultiplyMM(const float * lhs, const float *rhs, float * out)
{
    for (int i=0 ; i<DIM_SIZE ; i++) 
    {
        register const float rhs_i0 = rhs[ I(i,0) ];
        register float ri0 = lhs[ I(0,0) ] * rhs_i0;
        register float ri1 = lhs[ I(0,1) ] * rhs_i0;
        register float ri2 = lhs[ I(0,2) ] * rhs_i0;
        register float ri3 = lhs[ I(0,3) ] * rhs_i0;
        for (int j=1 ; j<DIM_SIZE ; j++) 
        {
            register const float rhs_ij = rhs[ I(i,j) ];
            ri0 += lhs[ I(j,0) ] * rhs_ij;
            ri1 += lhs[ I(j,1) ] * rhs_ij;
            ri2 += lhs[ I(j,2) ] * rhs_ij;
            ri3 += lhs[ I(j,3) ] * rhs_ij;
        }
        out[ I(i,0) ] = ri0;
        out[ I(i,1) ] = ri1;
        out[ I(i,2) ] = ri2;
        out[ I(i,3) ] = ri3;
    }
}

void Matrix4x4::Transpose()
{
    float temp[DATA_SIZE];    
    Transpose(data, temp);
    memcpy(temp, data, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::Transpose(const float* source, float *dest)
{
    for (int i = 0; i < DIM_SIZE; i++) 
    {
        int mBase = i * DIM_SIZE;
        dest[i] = source[mBase];
        dest[i + DIM_SIZE] = source[mBase + 1];
        dest[i + DIM_SIZE*2] = source[mBase + 2];
        dest[i + DIM_SIZE*3] = source[mBase + 3];
    }
}

void Matrix4x4::Invert()
{
    float temp[DATA_SIZE];    
    Invert(data, temp);
    memcpy(temp, data, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::Invert(const float * source, float * dest)
{
    // array of transpose source matrix
    float src[DATA_SIZE];

    // transpose matrix
    Transpose(source, src);

    // temp array for pairs
    float tmp[DATA_SIZE];

    // calculate pairs for first 8 elements (cofactors)
    tmp[0] = src[10] * src[15];

    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];

    // Holds the destination matrix while we're building it up.
    float dst[DATA_SIZE];

    // calculate first 8 elements (cofactors)
    dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
    dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
    dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
    dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
    dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
    dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
    dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
    dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
    dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
    dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
    dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
    dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
    dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
    dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
    dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
    dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

    // calculate pairs for second 8 elements (cofactors)
    tmp[0] = src[2] * src[7];
    tmp[1] = src[3] * src[6];
    tmp[2] = src[1] * src[7];
    tmp[3] = src[3] * src[5];
    tmp[4] = src[1] * src[6];
    tmp[5] = src[2] * src[5];
    tmp[6] = src[0] * src[7];
    tmp[7] = src[3] * src[4];
    tmp[8] = src[0] * src[6];
    tmp[9] = src[2] * src[4];
    tmp[10] = src[0] * src[5];
    tmp[11] = src[1] * src[4];

    // calculate second 8 elements (cofactors)
    dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
    dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
    dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
    dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
    dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
    dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
    dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
    dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
    dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
    dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
    dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
    dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
    dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
    dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
    dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
    dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

    // calculate determinant
    float det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

    if (det == 0.0f) 
    {

    }

    // calculate matrix inverse
    det = 1 / det;
    for (int j = 0; j < DATA_SIZE; j++)
        dest[j] = dst[j] * det;
}

void Matrix4x4::SetIdentity()
{
    SetIdentity(data);
}

void Matrix4x4::SetIdentity(float * target)
{
    for (int i=0 ; i< DATA_SIZE; i++) 
    {
        target[i] = 0;
    }

    for(int i = 0; i < DATA_SIZE; i += 5) 
    {
        target[i] = 1.0f;
    }
}

const float * Matrix4x4::GetDataPtr() const
{
    return (const float *)data;
}

void Matrix4x4::Scale(float x, float y, float z)
{
    float temp[DATA_SIZE];
    Scale(this->data, data, x, y, z);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::Scale(Matrix4x4 * out, float x, float y, float z) const
{
    Scale(this->data, out->data, x, y, z);
}

void Matrix4x4::Scale(const float * source, float * dest,  float x, float y, float z)
{
    for (int i=0 ; i<4 ; i++) 
    {
        int smi = i;
        int mi = i;
        dest[smi] = source[mi] * x;
        dest[4 + smi] = source[4 + mi] * y;
        dest[8 + smi] = source[8 + mi] * z;
        dest[12 + smi] = source[12 + mi];
    }
}

void Matrix4x4::Translate(const Vector3 * vector)
{
    float x = vector->x;
    float y = vector->y;
    float z = vector->x;
    Translate(x,y,z);
}

void Matrix4x4::Translate(float x, float y, float z)
{
    /*for (int i=0 ; i<4 ; i++) 
    {
        int mi = i;
        data[12 + mi] += data[mi] * x + data[4 + mi] * y + data[8 + mi] * z;
    }*/
    
    Translate(data, data, x, y, z);
}

void Matrix4x4::Translate(const Matrix4x4 * source, Matrix4x4 * out, const Vector3 * vector)
{
    Translate(source->data, out->data, vector->x, vector->y, vector->z);
}

void Matrix4x4::Translate(const Matrix4x4 * source, Matrix4x4 * out,float x, float y, float z)
{
    Translate(source->data, out->data, x, y, z);
}

void Matrix4x4::Translate(const float * source, float * dest, float x, float y, float z)
{
    if(source != dest)
    {
        for (int i=0 ; i<12 ; i++) 
        {
            dest[i] = source[i];
        }
    }
    
    for (int i=0 ; i<4 ; i++) 
    {
        int tmi = i;
        int mi = i;
        dest[12 + tmi] = source[mi] * x + source[4 + mi] * y + source[8 + mi] * z + source[12 + mi];
    }
}


void Matrix4x4::Rotate(const Vector3 * vector, float a)
{
    float temp[DATA_SIZE];  
    float r[DATA_SIZE];  
    SetRotate(r, vector->x, vector->y, vector->z, a);
    MultiplyMM(data, r, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::Rotate(float x, float y, float z, float a)
{
    float temp[DATA_SIZE];  
    float r[DATA_SIZE];  
    SetRotate(r, x, y, z, a);
    MultiplyMM(data, r, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

void Matrix4x4::SetRotateEuler(float x, float y, float z)
{
    SetRotateEuler(data, x, y, z);
}

void Matrix4x4::SetRotate(float * rm, float x, float y, float z, float a)
{
    rm[3] = 0;
    rm[7] = 0;
    rm[11]= 0;
    rm[12]= 0;
    rm[13]= 0;
    rm[14]= 0;
    rm[15]= 1;
    a *= (float) (PI/ 180.0f);
    float s = (float) sin(a);
    float c = (float) cos(a);
    if (1.0f == x && 0.0f == y && 0.0f == z) 
    {
        rm[5] = c;   rm[10]= c;
        rm[6] = s;   rm[9] = -s;
        rm[1] = 0;   rm[2] = 0;
        rm[4] = 0;   rm[8] = 0;
        rm[0] = 1;
    } 
    else if (0.0f == x && 1.0f == y && 0.0f == z) 
    {
        rm[0] = c;   rm[10]= c;
        rm[8] = s;   rm[2] = -s;
        rm[1] = 0;   rm[4] = 0;
        rm[6] = 0;   rm[9] = 0;
        rm[5] = 1;
    }
    else if (0.0f == x && 0.0f == y && 1.0f == z) 
    {
        rm[0] = c;   rm[5] = c;
        rm[1] = s;   rm[4] = -s;
        rm[2] = 0;   rm[6] = 0;
        rm[8] = 0;   rm[9] = 0;
        rm[10]= 1;
    } 
    else 
    {
        float len = Vector3::Magnitude(x, y, z);
        if (1.0f != len)  
        {
            float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        rm[ 0] = x*x*nc +  c;
        rm[ 4] =  xy*nc - zs;
        rm[ 8] =  zx*nc + ys;
        rm[ 1] =  xy*nc + zs;
        rm[ 5] = y*y*nc +  c;
        rm[ 9] =  yz*nc - xs;
        rm[ 2] =  zx*nc - ys;
        rm[ 6] =  yz*nc + xs;
        rm[10] = z*z*nc +  c;
    }
}

void Matrix4x4::SetRotateEuler(float * rm, float x, float y, float z)
{
    float piOver180 = PI / 180.0f;
    x *= piOver180;
    y *= piOver180;
    z *= piOver180;
    float cx = (float) cos(x);
    float sx = (float) sin(x);
    float cy = (float) cos(y);
    float sy = (float) sin(y);
    float cz = (float) cos(z);
    float sz = (float) sin(z);
    float cxsy = cx * sy;
    float sxsy = sx * sy;

    rm[0]  =   cy * cz;
    rm[1]  =  -cy * sz;
    rm[2]  =   sy;
    rm[3]  =  0.0f;

    rm[4]  =  cxsy * cz + cx * sz;
    rm[5]  = -cxsy * sz + cx * cz;
    rm[6]  =  -sx * cy;
    rm[7]  =  0.0f;

    rm[8]  = -sxsy * cz + sx * sz;
    rm[9]  =  sxsy * sz + sx * cz;
    rm[10] =  cx * cy;
    rm[11] =  0.0f;

    rm[12] =  0.0f;
    rm[13] =  0.0f;
    rm[14] =  0.0f;
    rm[15] =  1.0f;
}
