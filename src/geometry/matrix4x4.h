/********************************************* 
*
* class: Matrix4x4
*
* author: Mark Kellogg, with significant portions derived from the Android
* OpenGL matrix API: android.opengl.Matrix.
*
* These methods operate on OpenGL ES format matrices and
* vectors stored in Real arrays. Matrices are 4 x 4 column-vector matrices 
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

#include "global/global.h"

namespace GTE
{
	// forward declarations
	class Point3;
	class Vector3;
	class Quaternion;

	class Matrix4x4
	{
		static const Int32 DATA_SIZE = 16;
		static const Int32 DIM_SIZE = 4;

		Real data[DATA_SIZE];
		void Init();

	public:

		Matrix4x4();
		Matrix4x4(const Matrix4x4 & source);
		Matrix4x4(const Real * sourceData);
		~Matrix4x4();

		Real& A0; Real& A1; Real& A2; Real& A3;
		Real& B0; Real& B1; Real& B2; Real& B3;
		Real& C0; Real& C1; Real& C2; Real& C3;
		Real& D0; Real& D1; Real& D2; Real& D3;

		Int32 GetDataSize() const;

		Matrix4x4& operator= (const Matrix4x4 & source);

		void SetTo(const Matrix4x4& src);
		void SetTo(const Real * sourceData);
		void Transpose();
		static void Transpose(const Real* source, Real *dest);
		Bool Invert();
		Bool Invert(Matrix4x4& out);
		static Bool Invert(const Real * source, Real * dest);
		void SetIdentity();
		static void SetIdentity(Real * target);

		void BuildFromComponents(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
		void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
		Bool IsAffine(void) const;
		static Bool IsAffine(const Real * data);

		Real CalculateDeterminant();

		void MultiplyByScalar(Real scalar);

		void Transform(const Vector3& vector, Vector3& out) const;
		void Transform(Vector3& vector) const;
		void Transform(const Point3& point, Point3& out) const;
		void Transform(Point3& point) const;
		void Transform(Real * vector4f) const;
		void Add(const Matrix4x4& matrix);
		void Multiply(const Matrix4x4& matrix);
		void PreMultiply(const Matrix4x4& matrix);
		void Multiply(const Matrix4x4& matrix, Matrix4x4& out) const;
		static void Multiply(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& out);
		static void MultiplyMV(const Real * lhsMat, const Real * rhsVec, Real * out);
		static void MultiplyMM(const Real * lhs, const Real *rhs, Real * out);

		void Translate(const Vector3& vector);
		void Translate(Real x, Real y, Real z);
		void PreTranslate(const Vector3& vector);
		void PreTranslate(Real x, Real y, Real z);
		static void Translate(const Matrix4x4& src, Matrix4x4& out, const Vector3& vector);
		static void Translate(const Matrix4x4& src, Matrix4x4& out, Real x, Real y, Real z);
		static void Translate(const Real * source, Real * dest, Real x, Real y, Real z);
		static void PreTranslate(const Real * source, Real * dest, Real x, Real y, Real z);

		void Rotate(const Vector3& vector, Real a);
		void Rotate(Real x, Real y, Real z, Real a);
		void PreRotate(const Vector3& vector, Real a);
		void PreRotate(Real x, Real y, Real z, Real a);
		void SetRotateEuler(Real x, Real y, Real z);
		static void SetRotate(Real * rm, Real x, Real y, Real z, Real a);
		static void SetRotate(Matrix4x4& m, Real x, Real y, Real z, Real a);
		static void SetRotateEuler(Real * rm, Real x, Real y, Real z);

		void Scale(const Vector3& scale);
		void Scale(Real x, Real y, Real z);
		void PreScale(Real x, Real y, Real z);
		void Scale(Matrix4x4& out, Real x, Real y, Real z) const;
		static void Scale(const Real * source, Real * dest, Real x, Real y, Real z);

		static inline void Mx4transform(Real x, Real y, Real z, Real w, const Real* pM, Real* pDest);

		const Real * GetDataPtr() const;
	};
}

#endif
