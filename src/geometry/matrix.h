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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vector3.h"
#include "point3.h"

class Matrix 
{
    static const int DATA_SIZE = 16;
    static const int DIM_SIZE = 4;

    float data[DATA_SIZE];
    void Init();

    public:
    
    Matrix();
    Matrix(float * data);
    ~Matrix();

    Matrix & operator= (const Matrix & source);

    void Multiply(Vector3 * vector, Vector3 * out);
    void Multiply(Point3 * point, Point3 * out);
    void Multiply(Matrix * matrix);
    void Multiply(Matrix * matrix, Matrix * out);
    static void Multiply(Matrix * lhs, Matrix *rhs, Matrix * out);
    static void MultiplyMV(float * lhsMat, float * rhsVec, float * out);
    static void MultiplyMM(float * lhs, float *rhs, float * out);

    void Transpose();
    static void Transpose(float* source, float *dest);

    void Invert();
    static void Invert(float * source, float * dest);

    void SetIdentity();
    static void SetIdentity(float * source);

    void Translate(Vector3 * vector);
    void Translate(float x, float y, float z);
    static void Translate(Matrix * src, Matrix * out, Vector3 * vector);
    static void Translate(Matrix * src, Matrix * out,float x, float y, float z);
    static void Translate(float * source, float * dest, float x, float y, float z);

    void Rotate(Vector3 * vector, float a);
    void Rotate(float x, float y, float z, float a);
    void SetRotateEuler(float x, float y, float z);
    static void SetRotate(float * rm, float x, float y, float z, float a);
    static void SetRotateEuler(float * rm, float x, float y, float z);

    void Scale(float x, float y, float z);
    void Scale(Matrix * out, float x, float y, float z);
    static void Scale(float * source, float * dest, float x, float y , float z);

    static inline void Mx4transform(float x, float y, float z, float w, const float* pM, float* pDest);

    float * GetDataPtr();
};

#endif
