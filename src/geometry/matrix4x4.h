/********************************************* 
*
* class: Matrix4x4
*
* author: Mark Kellogg, with significant portions derived from the Android
* OpenGL matrix API: android.opengl.Matrix.
*
* These methods operate on OpenGL ES format matrices and
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

// forward declarations
class Point3;
class Vector3;
class Quaternion;

class Matrix4x4 
{
    static const int DATA_SIZE = 16;
    static const int DIM_SIZE = 4;

    float data[DATA_SIZE];
    void Init();

    public:
    
    Matrix4x4();
    Matrix4x4(const Matrix4x4 & source);
    Matrix4x4(float * data);
    ~Matrix4x4();

    float& A0; float& A1; float& A2; float& A3;
    float& B0; float& B1; float& B2; float& B3;
    float& C0; float& C1; float& C2; float& C3;
    float& D0; float& D1; float& D2; float& D3;

    int GetDataSize() const;

    Matrix4x4 & operator= (const Matrix4x4 & source);

    void SetTo(const Matrix4x4 * src);
    void SetTo(const float * data);

    void BuildFromComponents(const Vector3 * translation,const Quaternion * rotation,  const Vector3 * scale);
    void Decompose(Vector3 * translation, Quaternion * rotation, Vector3 * scale);

    bool IsAffine(void) const;

    void MultiplyByScalar(float scalar);

    void Transform(const Vector3 * vector, Vector3 * out) const;
    void Transform(Vector3 * vector) const;
    void Transform(const Point3 * point, Point3 * out) const;
    void Transform(Point3 * point) const;
    void Transform(float * vector4f) const;
    void Add(const Matrix4x4 * matrix);
    void Multiply(const Matrix4x4 * matrix);
    void PreMultiply(const Matrix4x4 * matrix);
    void Multiply(const Matrix4x4 * matrix, Matrix4x4 * out) const;
    static void Multiply(const Matrix4x4 * lhs, const Matrix4x4 *rhs, Matrix4x4 * out);
    static void MultiplyMV(const float * lhsMat, const float * rhsVec, float * out);
    static void MultiplyMM(const float * lhs, const float *rhs, float * out);

    void Transpose();
    static void Transpose(const float* source, float *dest);

    bool Invert();
    bool Invert(Matrix4x4 * out);
    static bool Invert(const float * source, float * dest);

    void SetIdentity();
    static void SetIdentity(float * target);

    void Translate(const Vector3 * vector);
    void Translate(float x, float y, float z);
    void PreTranslate(const Vector3 * vector);
    void PreTranslate(float x, float y, float z);
    static void Translate(const Matrix4x4 * src, Matrix4x4 * out, const Vector3 * vector);
    static void Translate(const Matrix4x4 * src, Matrix4x4 * out,float x, float y, float z);
    static void Translate(const float * source, float * dest, float x, float y, float z);
    static void PreTranslate(const float * source, float * dest, float x, float y, float z);

    void Rotate(const Vector3 * vector, float a);
    void Rotate(float x, float y, float z, float a);
    void PreRotate(const Vector3 * vector, float a);
    void PreRotate(float x, float y, float z, float a);
    void SetRotateEuler(float x, float y, float z);
    static void SetRotate(float * rm, float x, float y, float z, float a);
    static void SetRotate(Matrix4x4 * m, float x, float y, float z, float a);
    static void SetRotateEuler(float * rm, float x, float y, float z);

    void Scale(float x, float y, float z);
    void PreScale(float x, float y, float z);
    void Scale(Matrix4x4 * out, float x, float y, float z) const ;
    static void Scale(const float * source, float * dest, float x, float y , float z);

    static inline void Mx4transform(float x, float y, float z, float w, const float* pM, float* pDest);

    const float * GetDataPtr() const;
};

#endif
