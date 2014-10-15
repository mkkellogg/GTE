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
#include "global/global.h"

#define I(_i, _j) ((_j)+ DIM_SIZE*(_i))
#define PI 3.14159265

/*
 * Default constructor
 */
Matrix4x4::Matrix4x4()
{
    Init();
    SetIdentity();
}

/*
 * Construct matrix with existing float data
 */
Matrix4x4::Matrix4x4(float * data)
{
    Init();
    if(data != NULL)
    {
    	memcpy(this->data, data, sizeof(float) * DATA_SIZE);
    }
    else
    {
    	SetIdentity();
    	Debug::PrintError("Matrix4x4::Matrix4x4(float *) -> NULL data passed.");
    }
}

/*
 * Clean up
 */
Matrix4x4::~Matrix4x4()
{
    
}

int Matrix4x4::GetDataSize() const
{
	return DATA_SIZE;
}

/*
 * Copy data from existing matrix to this one
 */
void Matrix4x4::SetTo(const Matrix4x4 * src)
{
	NULL_CHECK_RTRN(src,"Matrix4x4::SetTo -> NULL pointer passed");
	if(this == src)return;
	SetTo(src->data);
}

/*
 * Copy existing matrix data (from a float array) to this one
 */
void Matrix4x4::SetTo(const float * srcData)
{
	NULL_CHECK_RTRN(srcData,"Matrix4x4::Set -> srcData is NULL");
	memcpy(data, srcData, sizeof(float) * DATA_SIZE);
}

/*
 * Overloaded assignment operator
 */
Matrix4x4 & Matrix4x4::operator= (const Matrix4x4 & source)
{
    if(this == &source)return *this;
    SetTo(&source);
    return *this;
}

/*
 * General initialization
 */
void Matrix4x4::Init()
{
    
}

/*
 * Multiply this matrix by the scalar [scalar]
 */
void Matrix4x4::MultiplyByScalar(float scalar)
{
	for(unsigned int i = 0; i < 16; i++)
	{
		data[i] *= scalar;
	}
}

/*
 * Transform [vector] by this matrix, and store the result in [out]
 */
void Matrix4x4::Transform(const Vector3 * vector, Vector3 * out) const
{
    MultiplyMV(this->data, const_cast<Vector3 *>(vector)->GetDataPtr(), out->GetDataPtr());
}

/*
 * Transform [point] by this matrix, and store the result in [out]
 */
void Matrix4x4::Transform(const Point3 * point, Point3 * out) const
{
    MultiplyMV(this->data,  const_cast<Point3 *>(point)->GetDataPtr(), out->GetDataPtr());
}

/*
 * Transform [vector] by this matrix
 */
void Matrix4x4::Transform(Vector3 * vector) const
{
	NULL_CHECK_RTRN(vector,"Matrix4x4::Transform(Vector3 *) -> vector is NULL");

	float temp[DIM_SIZE];
	MultiplyMV(this->data, vector->GetDataPtr(), temp);
	memcpy(vector->GetDataPtr(), temp, sizeof(float) * DIM_SIZE);
}

/*
 * Transform [point] by this matrix
 */
void Matrix4x4::Transform(Point3 * point) const
{
	float temp[DIM_SIZE];
	MultiplyMV(this->data, point->GetDataPtr(), temp);
	memcpy(point->GetDataPtr(), temp, sizeof(float) * DIM_SIZE);
}

/*
 * Transform [vector4f] by this matrix
 */
void Matrix4x4::Transform(float * vector4f) const
{
	NULL_CHECK_RTRN(vector4f, "Matrix4x4::Transform(float *) -> vector4f is NULL");

	float temp[DIM_SIZE];
	MultiplyMV(this->data, vector4f, temp);
	memcpy(vector4f, temp, sizeof(float) * DIM_SIZE);
}

/*
 * Add [matrix] to this matrix
 */
void Matrix4x4::Add(const Matrix4x4 * matrix)
{
	NULL_CHECK_RTRN(matrix, "Matrix4x4::add -> matrix is NULL");
	for(unsigned int i=0; i < 16; i++)
	{
		data[i] += matrix->data[i];
	}
}

/*
 * Post-multiply this matrix by [matrix]
 */
void Matrix4x4::Multiply(const Matrix4x4 * matrix)
{
	NULL_CHECK_RTRN(matrix, "Matrix4x4::Multiply(Matrix4x4 *) -> matrix is NULL");

    float temp[DATA_SIZE];
    MultiplyMM(this->data, matrix->data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Pre-multiply this matrix by [matrix]
 */
void Matrix4x4::PreMultiply(const Matrix4x4 * matrix)
{
	NULL_CHECK_RTRN(matrix, "Matrix4x4::PreMultiply(Matrix4x4 *) -> matrix is NULL");

    float temp[DATA_SIZE];
    MultiplyMM(matrix->data, this->data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Post-multiply this matrix by [matrix], and store the result in [out]
 */
void Matrix4x4::Multiply(const Matrix4x4 * matrix, Matrix4x4 * out) const
{
	NULL_CHECK_RTRN(matrix, "Matrix4x4::Multiply(Matrix4x4 *, Matrix4x4 *) -> matrix is NULL");
	NULL_CHECK_RTRN(out, "Matrix4x4::Multiply(Matrix4x4 *, Matrix4x4 *) -> out is NULL");

    MultiplyMM(this->data, matrix->data, out->data);
}

/*
 * Post-multiply [lhs] by [rhs], and store the result in [out]
 */
void Matrix4x4::Multiply(const Matrix4x4 * lhs, const Matrix4x4 *rhs, Matrix4x4 * out)
{
	NULL_CHECK_RTRN(lhs, "Matrix4x4::Multiply(Matrix4x4 *, Matrix4x4 *,  Matrix4x4 *) -> lhs is NULL");
	NULL_CHECK_RTRN(rhs, "Matrix4x4::Multiply(Matrix4x4 *, Matrix4x4 *,  Matrix4x4 *) -> rhs is NULL");
	NULL_CHECK_RTRN(out, "Matrix4x4::Multiply(Matrix4x4 *, Matrix4x4 *,  Matrix4x4 *) -> out is NULL");

    MultiplyMM(lhs->data, rhs->data, out->data);
}

/*
 * Transform the vector pointed to by [rhsVec], by the matrix pointed to by [lhsMat],
 * and store the result in [out]
 */
void Matrix4x4::MultiplyMV(const float * lhsMat, const float * rhsVec, float * out)
{
	NULL_CHECK_RTRN(lhsMat, "Matrix4x4::MultiplyMV -> lhsMat is NULL");
	NULL_CHECK_RTRN(rhsVec, "Matrix4x4::MultiplyMV -> rhsVec is NULL");
	NULL_CHECK_RTRN(out, "Matrix4x4::MultiplyMV -> out is NULL");

    Mx4transform(rhsVec[0], rhsVec[1], rhsVec[2], rhsVec[3], lhsMat, out);
}

/*
 * Transform the homogeneous point/vector specified by [x,y,z,w], by the matrix pointed to by [matrix],
 * and store the result in [pDest]
 */
void Matrix4x4::Mx4transform(float x, float y, float z, float w, const float* matrix, float* pDest) 
{
	NULL_CHECK_RTRN(matrix, "Matrix4x4::Mx4transform -> lhsMat is NULL");
	NULL_CHECK_RTRN(pDest, "Matrix4x4::Mx4transform -> pDest is NULL");

    pDest[0] = matrix[0 + 4 * 0] * x + matrix[0 + 4 * 1] * y + matrix[0 + 4 * 2] * z + matrix[0 + 4 * 3] * w;
    pDest[1] = matrix[1 + 4 * 0] * x + matrix[1 + 4 * 1] * y + matrix[1 + 4 * 2] * z + matrix[1 + 4 * 3] * w;
    pDest[2] = matrix[2 + 4 * 0] * x + matrix[2 + 4 * 1] * y + matrix[2 + 4 * 2] * z + matrix[2 + 4 * 3] * w;
    pDest[3] = matrix[3 + 4 * 0] * x + matrix[3 + 4 * 1] * y + matrix[3 + 4 * 2] * z + matrix[3 + 4 * 3] * w;
}

/*********************************************************
*
* Multiply two 4x4 matrices (in float array form) together and store the result in a third 4x4 matrix.
* In matrix notation: out = lhs x rhs. Due to the way matrix multiplication works, 
* the [out] matrix will have the same effect as first multiplying by the [rhs] matrix,
* then multiplying by the [lhs] matrix.
* 
* Parameters:
* [out] The float array that holds the result.
* [lhs] The float array that holds the left-hand-side 4x4 matrix.
* [rhs] The float array that holds the right-hand-side 4x4 matrix.
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

/*
 * Transpose this matrix
 */
void Matrix4x4::Transpose()
{
    float temp[DATA_SIZE];    
    Transpose(data, temp);
    memcpy(temp, data, sizeof(float) * DATA_SIZE);
}

/*
 * Transpose the 4x4 matrix pointed to by [source] and store in [dest].
 */
void Matrix4x4::Transpose(const float* source, float *dest)
{
	NULL_CHECK_RTRN(source, "Matrix4x4::Transpose -> source is NULL");
	NULL_CHECK_RTRN(dest, "Matrix4x4::Transpose -> dest is NULL");

    for (int i = 0; i < DIM_SIZE; i++) 
    {
        int mBase = i * DIM_SIZE;
        dest[i] = source[mBase];
        dest[i + DIM_SIZE] = source[mBase + 1];
        dest[i + DIM_SIZE*2] = source[mBase + 2];
        dest[i + DIM_SIZE*3] = source[mBase + 3];
    }
}

/*
 * Invert this matrix.
 *
 * Returns false if the matrix cannot be inverted
 */
bool Matrix4x4::Invert()
{
    float temp[DATA_SIZE];    
    bool success = Invert(data, temp);
    if(success == true)
    {
    	memcpy(data, temp, sizeof(float) * DATA_SIZE);
    }
    return success;
}

/*
 * Invert the 4x4 matrix pointed to by [out].
 *
 * Returns false if the matrix cannot be inverted
 */
bool Matrix4x4::Invert(Matrix4x4 * out)
{
	NULL_CHECK(out, "Matrix4x4::Invert -> out is NULL", false);
	return Invert(data, out->data);
}

/*
 * Invert the 4x4 matrix pointed to by [source] and store the result in [dest]
 *
 * Returns false if the matrix cannot be inverted
 */
bool Matrix4x4::Invert(const float * source, float * dest)
{
	NULL_CHECK(source, "Matrix4x4::Invert -> source is NULL", false);
	NULL_CHECK(dest, "Matrix4x4::Invert -> dest is NULL", false);

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
    	return false;
    }

    // calculate matrix inverse
    det = 1 / det;
    for (int j = 0; j < DATA_SIZE; j++)
        dest[j] = dst[j] * det;

    return true;
}

/*
 * Set this matrix to the identity matrix
 */
void Matrix4x4::SetIdentity()
{
    SetIdentity(data);
}

/*
 * Set the 4x4 matrix pointed to by [target]  to the identity matrix.
 */
void Matrix4x4::SetIdentity(float * target)
{
	NULL_CHECK_RTRN(target, "Matrix4x4::SetIdentity -> target is NULL");

    for (int i=0 ; i< DATA_SIZE; i++) 
    {
        target[i] = 0;
    }

    for(int i = 0; i < DATA_SIZE; i += 5) 
    {
        target[i] = 1.0f;
    }
}

/*
 * Get a pointer the float data that makes up this matrix
 */
const float * Matrix4x4::GetDataPtr() const
{
    return (const float *)data;
}

/*
 * Scale this matrix by [x], [y], and [z].
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying this
 * matrix by a scale matrix
 */
void Matrix4x4::Scale(float x, float y, float z)
{
    float temp[DATA_SIZE];
    Scale(this->data, temp, x, y, z);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Scale this matrix by [x], [y], and [z].
 *
 * This performs a pre-transformation, in that it is equivalent to pre-multiplying this
 * matrix by a scale matrix
 */
void Matrix4x4::PreScale(float x, float y, float z)
{
	float temp[DATA_SIZE];
	Matrix4x4 scale;
	scale.SetIdentity();
	scale.Scale(x,y,z);

	MultiplyMM(scale.GetDataPtr(), this->data, temp);

	memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Scale this matrix by [x], [y], and [z] and store the result in [out]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying this
 * matrix by a scale matrix
 */
void Matrix4x4::Scale(Matrix4x4 * out, float x, float y, float z) const
{
    Scale(this->data, out->data, x, y, z);
}

/*
 * Scale the matrix pointed to by [source] by [x], [y], and [z], and store
 * the result in [dest]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * [source] by a scale matrix
 */
void Matrix4x4::Scale(const float * source, float * dest,  float x, float y, float z)
{
	NULL_CHECK_RTRN(source, "Matrix4x4::Scale -> source is NULL");
	NULL_CHECK_RTRN(dest, "Matrix4x4::Scale -> dest is NULL");

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

/*
 * Translate this matrix by [vector]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * this matrix by a translation matrix
 */
void Matrix4x4::Translate(const Vector3 * vector)
{
	NULL_CHECK_RTRN(vector, "Matrix4x4::Translate -> vector is NULL");

    float x = vector->x;
    float y = vector->y;
    float z = vector->x;
    Translate(x,y,z);
}

/*
 * Translate this matrix by [x], [y], [z]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * this matrix by a translation matrix
 */
void Matrix4x4::Translate(float x, float y, float z)
{
    Translate(data, data, x, y, z);
}

/*
 * Translate this matrix by [x], [y], [z]
 *
 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
 * this matrix by a translation matrix
 */
void Matrix4x4::PreTranslate(float x, float y, float z)
{
	float dest[DATA_SIZE];
	Matrix4x4::PreTranslate(data,dest, x,y,z);
	memcpy(data, dest, sizeof(float)*DATA_SIZE);
}

/*
 * Translate the 4x4 matrix pointed to by [source] by [vector], and store in [out]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * [source] by a translation matrix
 */
void Matrix4x4::Translate(const Matrix4x4 * source, Matrix4x4 * out, const Vector3 * vector)
{
    Translate(source->data, out->data, vector->x, vector->y, vector->z);
}

/*
 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [out]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * [source] by a translation matrix
 */
void Matrix4x4::Translate(const Matrix4x4 * source, Matrix4x4 * out,float x, float y, float z)
{
    Translate(source->data, out->data, x, y, z);
}

/*
 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [dest]
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * [source] by a translation matrix
 */
void Matrix4x4::Translate(const float * source, float * dest, float x, float y, float z)
{
	NULL_CHECK_RTRN(source, "Matrix4x4::Translate -> source is NULL");

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

/*
 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [dest]
 *
 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
 * [source] by a translation matrix
 */
void Matrix4x4::PreTranslate(const float * source, float * dest, float x, float y, float z)
{
    Matrix4x4 trans;
    trans.SetIdentity();

    float *matrixData = const_cast<float *>(trans.GetDataPtr());
    matrixData[12] = x;
    matrixData[13] = y;
    matrixData[14] = z;

    Matrix4x4::MultiplyMM(matrixData, source, dest);
}

/*
 * Rotate this matrix around the [vector] axis by [a] degrees
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * this matrix by a rotation matrix
 */
void Matrix4x4::Rotate(const Vector3 * vector, float a)
{
	NULL_CHECK_RTRN(vector, "Matrix4x4::Rotate -> vector is NULL");

    float temp[DATA_SIZE];  
    float r[DATA_SIZE];  
    SetRotate(r, vector->x, vector->y, vector->z, a);
    MultiplyMM(data, r, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Rotate this matrix around the [x], [y], [z] axis by [a] degrees
 *
 * This performs a post-transformation, in that it is equivalent to post-multiplying
 * this matrix by a rotation matrix
 */
void Matrix4x4::Rotate(float x, float y, float z, float a)
{
    float temp[DATA_SIZE];  
    float r[DATA_SIZE];  
    SetRotate(r, x, y, z, a);
    MultiplyMM(data, r, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Rotate this matrix around the [vector] axis by [a] degrees
 *
 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
 * this matrix by a rotation matrix
 */
void Matrix4x4::PreRotate(const Vector3 * vector, float a)
{
	NULL_CHECK_RTRN(vector, "Matrix4x4::PreRotate -> vector is NULL");

    float temp[DATA_SIZE];
    float r[DATA_SIZE];
    SetRotate(r, vector->x, vector->y, vector->z, a);
    MultiplyMM(r, data,temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Rotate this matrix around the [x], [y], [z] axis by [a] degrees
 *
 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
 * this matrix by a rotation matrix
 */

void Matrix4x4::PreRotate(float x, float y, float z, float a)
{
    float temp[DATA_SIZE];
    float r[DATA_SIZE];
    SetRotate(r, x, y, z, a);
    MultiplyMM(r, data, temp);
    memcpy(data, temp, sizeof(float) * DATA_SIZE);
}

/*
 * Set this matrix to be a rotation matrix, with Euler angles [x], [y], [z]
 */
void Matrix4x4::SetRotateEuler(float x, float y, float z)
{
    SetRotateEuler(data, x, y, z);
}

/*
 * Set the 4x4 matrix [rm] to be a rotation matrix, around axis [x], [y], [z] by [a] degrees
 */
void Matrix4x4::SetRotate(float * rm, float x, float y, float z, float a)
{
	NULL_CHECK_RTRN(rm, "Matrix4x4::SetRotate -> rm is NULL");

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

/*
 * Set the matrix [rm] to be a rotation matrix, with Euler angles [x], [y], [z]
 */
void Matrix4x4::SetRotateEuler(float * rm, float x, float y, float z)
{
	NULL_CHECK_RTRN(rm, "Matrix4x4::SetRotateEuler -> rm is NULL");

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
