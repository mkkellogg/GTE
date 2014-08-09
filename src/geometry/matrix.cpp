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
 
#include "matrix.h"
#include "point3.h"
#include "vector3.h"

#define I(_i, _j) ((_j)+ DIM_SIZE*(_i))

Matrix::Matrix()
{
    Init();
    SetIdentity();
}

Matrix::Matrix(float * data)
{
    Init();
    memcpy(this->data, data, sizeof(float) * DATA_SIZE);
}

Matrix::~Matrix()
{
    
}

Matrix & Matrix::operator= (const Matrix & source)
{
    if(this == &source)return *this;
    memcpy((void*)source.data, (void*)data, sizeof(float) * DATA_SIZE); 
    return *this;
}

void Matrix::Init()
{
    
}

void Matrix::Multiply(Vector3 * vector, Vector3 * out)
{
    MultiplyMV(this->data, vector->GetDataPtr(), out->GetDataPtr());
}

void Matrix::Multiply(Point3 * point, Point3 * out)
{
    MultiplyMV(this->data, point->GetDataPtr(), out->GetDataPtr());
}

void Matrix::Multiply(Matrix * matrix)
{
    float temp[DATA_SIZE];
    MultiplyMM(matrix->data, this->data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

void Matrix::Multiply(Matrix * matrix, Matrix * out)
{
    MultiplyMM(matrix->data, this->data, out->data);
}

void Matrix::Multiply(Matrix * lhs, Matrix *rhs, Matrix * out)
{
    MultiplyMM(lhs->data, rhs->data, out->data);
}

void Matrix::MultiplyMV(float * lhsMat, float * rhsVec, float * out)
{
    Mx4transform(rhsVec[0], rhsVec[1], rhsVec[2], rhsVec[3], lhsMat, out);
}

void Matrix::Mx4transform(float x, float y, float z, float w, const float* pM, float* pDest) 
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

void Matrix::MultiplyMM(float * lhs, float *rhs, float * out)
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

void Matrix::Transpose()
{
    float temp[DATA_SIZE];    
    Transpose(data, temp);
    memcpy(temp, data, sizeof(float) * DATA_SIZE);
}

void Matrix::Transpose(float* source, float *dest)
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

void Matrix::Invert()
{
    float temp[DATA_SIZE];    
    Invert(data, temp);
    memcpy(temp, data, sizeof(float) * DATA_SIZE);
}

void Matrix::Invert(float * source, float * dest)
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

void Matrix::SetIdentity()
{
    SetIdentity(data);
}

void Matrix::SetIdentity(float * source)
{
    for (int i=0 ; i< DATA_SIZE; i++) 
    {
        source[i] = 0;
    }

    for(int i = 0; i < DATA_SIZE; i += 5) 
    {
        source[i] = 1.0f;
    }
}

float * Matrix::GetDataPtr()
{
    return data;
}

void Matrix::Translate(Vector3 * vector)
{
    float x = vector->x;
    float y = vector->y;
    float z = vector->x;
    Translate(x,y,z);
}

void Matrix::Translate(float x, float y, float z)
{
    for (int i=0 ; i<4 ; i++) 
    {
        int mi = i;
        data[12 + mi] += data[mi] * x + data[4 + mi] * y + data[8 + mi] * z;
    }
}

void Matrix::Translate(Matrix * source, Matrix * out, Vector3 * vector)
{
    Translate(source->data, out->data, vector->x, vector->y, vector->z);
}

void Matrix::Translate(Matrix * source, Matrix * out,float x, float y, float z)
{
    Translate(source->data, out->data, x, y, z);
}

void Matrix::Translate(float * source, float * dest, float x, float y, float z)
{
    for (int i=0 ; i<12 ; i++) 
    {
        dest[i] = source[i];
    }
    
    for (int i=0 ; i<4 ; i++) 
    {
        int tmi = i;
        int mi = i;
        dest[12 + tmi] = source[mi] * x + source[4 + mi] * y + source[8 + mi] * z + source[12 + mi];
    }
}

public static void More ...setRotateM(float[] rm, int rmOffset,
487            float a, float x, float y, float z) {
488        rm[rmOffset + 3] = 0;
489        rm[rmOffset + 7] = 0;
490        rm[rmOffset + 11]= 0;
491        rm[rmOffset + 12]= 0;
492        rm[rmOffset + 13]= 0;
493        rm[rmOffset + 14]= 0;
494        rm[rmOffset + 15]= 1;
495        a *= (float) (Math.PI / 180.0f);
496        float s = (float) Math.sin(a);
497        float c = (float) Math.cos(a);
498        if (1.0f == x && 0.0f == y && 0.0f == z) {
499            rm[rmOffset + 5] = c;   rm[rmOffset + 10]= c;
500            rm[rmOffset + 6] = s;   rm[rmOffset + 9] = -s;
501            rm[rmOffset + 1] = 0;   rm[rmOffset + 2] = 0;
502            rm[rmOffset + 4] = 0;   rm[rmOffset + 8] = 0;
503            rm[rmOffset + 0] = 1;
504        } else if (0.0f == x && 1.0f == y && 0.0f == z) {
505            rm[rmOffset + 0] = c;   rm[rmOffset + 10]= c;
506            rm[rmOffset + 8] = s;   rm[rmOffset + 2] = -s;
507            rm[rmOffset + 1] = 0;   rm[rmOffset + 4] = 0;
508            rm[rmOffset + 6] = 0;   rm[rmOffset + 9] = 0;
509            rm[rmOffset + 5] = 1;
510        } else if (0.0f == x && 0.0f == y && 1.0f == z) {
511            rm[rmOffset + 0] = c;   rm[rmOffset + 5] = c;
512            rm[rmOffset + 1] = s;   rm[rmOffset + 4] = -s;
513            rm[rmOffset + 2] = 0;   rm[rmOffset + 6] = 0;
514            rm[rmOffset + 8] = 0;   rm[rmOffset + 9] = 0;
515            rm[rmOffset + 10]= 1;
516        } else {
517            float len = length(x, y, z);
518            if (1.0f != len) {
519                float recipLen = 1.0f / len;
520                x *= recipLen;
521                y *= recipLen;
522                z *= recipLen;
523            }
524            float nc = 1.0f - c;
525            float xy = x * y;
526            float yz = y * z;
527            float zx = z * x;
528            float xs = x * s;
529            float ys = y * s;
530            float zs = z * s;
531            rm[rmOffset +  0] = x*x*nc +  c;
532            rm[rmOffset +  4] =  xy*nc - zs;
533            rm[rmOffset +  8] =  zx*nc + ys;
534            rm[rmOffset +  1] =  xy*nc + zs;
535            rm[rmOffset +  5] = y*y*nc +  c;
536            rm[rmOffset +  9] =  yz*nc - xs;
537            rm[rmOffset +  2] =  zx*nc - ys;
538            rm[rmOffset +  6] =  yz*nc + xs;
539            rm[rmOffset + 10] = z*z*nc +  c;
540        }
541    }

