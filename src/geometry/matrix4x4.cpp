/********************************************* 
*
* Matrix math utilities. These methods operate on OpenGL ES format matrices and 
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "debug/gtedebug.h"
#include "matrix4x4.h"
#include "quaternion.h"
#include "point/point3.h"
#include "vector/vector3.h"
#include "global/global.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"

#define I(_i, _j) ((_j)+ DIM_SIZE*(_i))
#define PI 3.14159265

namespace GTE
{
	/*
	* Default constructor
	*/
	Matrix4x4::Matrix4x4() : A0(data[0]), A1(data[4]), A2(data[8]), A3(data[12]),
		B0(data[1]), B1(data[5]), B2(data[9]), B3(data[13]),
		C0(data[2]), C1(data[6]), C2(data[10]), C3(data[14]),
		D0(data[3]), D1(data[7]), D2(data[11]), D3(data[15])
	{
		Init();
		SetIdentity();
	}

	/*
	 * Construct matrix with existing Real data
	 */
	Matrix4x4::Matrix4x4(const Real * sourceData) : A0(data[0]), A1(data[4]), A2(data[8]), A3(data[12]),
		B0(data[1]), B1(data[5]), B2(data[9]), B3(data[13]),
		C0(data[2]), C1(data[6]), C2(data[10]), C3(data[14]),
		D0(data[3]), D1(data[7]), D2(data[11]), D3(data[15])
	{
		Init();

		if (sourceData != NULL)
		{
			memcpy(data, sourceData, sizeof(Real) * DATA_SIZE);
		}
		else
		{
			SetIdentity();
			Debug::PrintWarning("Matrix4x4::Matrix4x4(Real *) -> NULL data passed.");
		}
	}

	/*
	 * Copy constructor.
	 */
	Matrix4x4::Matrix4x4(const Matrix4x4& source) : A0(data[0]), A1(data[4]), A2(data[8]), A3(data[12]),
		B0(data[1]), B1(data[5]), B2(data[9]), B3(data[13]),
		C0(data[2]), C1(data[6]), C2(data[10]), C3(data[14]),
		D0(data[3]), D1(data[7]), D2(data[11]), D3(data[15])
	{
		SetTo(source);
	}

	/*
	 * Clean up
	 */
	Matrix4x4::~Matrix4x4()
	{

	}

	Int32 Matrix4x4::GetDataSize() const
	{
		return DATA_SIZE;
	}

	/*
	 * Copy data from existing matrix to this one
	 */
	void Matrix4x4::SetTo(const Matrix4x4& src)
	{
		if (this == &src)return;
		SetTo(src.data);
	}

	/*
	 * Copy existing matrix data (from a Real array) to this one
	 */
	void Matrix4x4::SetTo(const Real * sourceData)
	{
		NONFATAL_ASSERT(sourceData != NULL, "Matrix4x4::SetTo -> 'srcData' is null", true);
		memcpy(data, sourceData, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Build matrix from components [translation], [scale], and [rotation]
	 */
	void Matrix4x4::BuildFromComponents(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		Matrix4x4 rotMatrix = rotation.rotationMatrix();

		// Build the final matrix, with translation, scale, and rotation
		A0 = scale.x * rotMatrix.A0; A1 = scale.y *  rotMatrix.A1; A2 = scale.z * rotMatrix.A2; A3 = translation.x;
		B0 = scale.x * rotMatrix.B0; B1 = scale.y *  rotMatrix.B1; B2 = scale.z * rotMatrix.B2; B3 = translation.y;
		C0 = scale.x * rotMatrix.C0; C1 = scale.y *  rotMatrix.C1; C2 = scale.z * rotMatrix.C2; C3 = translation.z;

		D0 = 0; D1 = 0; D2 = 0; D3 = 1;
	}

	void Matrix4x4::Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const
	{
		NONFATAL_ASSERT(IsAffine(), "Matrix4x4::Decompose -> Matrix is not affine.", true);

		Matrix4x4 rotMatrix;

		// build orthogonal matrix [rotMatrix]
		Real fInvLength = GTEMath::InverseSquareRoot(A0*A0 + B0*B0 + C0*C0);

		rotMatrix.A0 = A0*fInvLength;
		rotMatrix.B0 = B0*fInvLength;
		rotMatrix.C0 = C0*fInvLength;

		Real fDot = rotMatrix.A0*A1 + rotMatrix.B0*B1 + rotMatrix.C0*C1;
		rotMatrix.A1 = A1 - fDot*rotMatrix.A0;
		rotMatrix.B1 = B1 - fDot*rotMatrix.B0;
		rotMatrix.C1 = C1 - fDot*rotMatrix.C0;
		fInvLength = GTEMath::InverseSquareRoot(rotMatrix.A1*rotMatrix.A1 + rotMatrix.B1*rotMatrix.B1 + rotMatrix.C1*rotMatrix.C1);

		rotMatrix.A1 *= fInvLength;
		rotMatrix.B1 *= fInvLength;
		rotMatrix.C1 *= fInvLength;

		fDot = rotMatrix.A0*A2 + rotMatrix.B0*B2 + rotMatrix.C0*C2;
		rotMatrix.A2 = A2 - fDot*rotMatrix.A0;
		rotMatrix.B2 = B2 - fDot*rotMatrix.B0;
		rotMatrix.C2 = C2 - fDot*rotMatrix.C0;

		fDot = rotMatrix.A1*A2 + rotMatrix.B1*B2 + rotMatrix.C1*C2;
		rotMatrix.A2 -= fDot*rotMatrix.A1;
		rotMatrix.B2 -= fDot*rotMatrix.B1;
		rotMatrix.C2 -= fDot*rotMatrix.C1;

		fInvLength = GTEMath::InverseSquareRoot(rotMatrix.A2*rotMatrix.A2 + rotMatrix.B2*rotMatrix.B2 + rotMatrix.C2*rotMatrix.C2);

		rotMatrix.A2 *= fInvLength;
		rotMatrix.B2 *= fInvLength;
		rotMatrix.C2 *= fInvLength;

		// guarantee that orthogonal matrix has determinant 1 (no reflections)
		Real fDet = rotMatrix.A0*rotMatrix.B1*rotMatrix.C2 + rotMatrix.A1*rotMatrix.B2*rotMatrix.C0 +
			rotMatrix.A2*rotMatrix.B0*rotMatrix.C1 - rotMatrix.A2*rotMatrix.B1*rotMatrix.C0 -
			rotMatrix.A1*rotMatrix.B0*rotMatrix.C2 - rotMatrix.A0*rotMatrix.B2*rotMatrix.C1;

		if (fDet < 0.0)
		{
			for (size_t iRow = 0; iRow < 3; iRow++)
				for (size_t iCol = 0; iCol < 3; iCol++)
					rotMatrix.data[iCol * DIM_SIZE + iRow] = -rotMatrix.data[iCol * DIM_SIZE + iRow];
		}

		// build "right" matrix [rightMatrix]
		Matrix4x4 rightMatrix;
		rightMatrix.A0 = rotMatrix.A0*A0 + rotMatrix.B0*B0 + rotMatrix.C0*C0;
		rightMatrix.A1 = rotMatrix.A0*A1 + rotMatrix.B0*B1 + rotMatrix.C0*C1;
		rightMatrix.B1 = rotMatrix.A1*A1 + rotMatrix.B1*B1 + rotMatrix.C1*C1;
		rightMatrix.A2 = rotMatrix.A0*A2 + rotMatrix.B0*B2 + rotMatrix.C0*C2;
		rightMatrix.B2 = rotMatrix.A1*A2 + rotMatrix.B1*B2 + rotMatrix.C1*C2;
		rightMatrix.C2 = rotMatrix.A2*A2 + rotMatrix.B2*B2 + rotMatrix.C2*C2;

		// the scaling component
		scale.x = rightMatrix.A0;
		scale.y = rightMatrix.B1;
		scale.z = rightMatrix.C2;

		Vector3 shear;

		// the shear component
		Real fInvD0 = 1.0f / scale.x;
		shear.x = rightMatrix.A1*fInvD0;
		shear.y = rightMatrix.A2*fInvD0;
		shear.z = rightMatrix.B2 / scale.y;

		rotation.FromMatrix(rotMatrix);
		translation.Set(A3, B3, C3);
	}

	bool Matrix4x4::IsAffine(void) const
	{
		return D0 == 0 && D1 == 0 && D2 == 0 && D3 == 1;
	}

	bool Matrix4x4::IsAffine(const Real * data)
	{
		return data[3] == 0 && data[7] == 0 && data[11] == 0 && data[15] == 1;
	}

	/*
	 * Overloaded assignment operator
	 */
	Matrix4x4 & Matrix4x4::operator= (const Matrix4x4& source)
	{
		if (this == &source)return *this;
		SetTo(source);
		return *this;
	}

	/*
	 * General initialization
	 */
	void Matrix4x4::Init()
	{

	}

	/*
	* Calculate the determinant of this matrix.
	*/
	Real Matrix4x4::CalculateDeterminant()
	{
		// array of transpose source matrix
		Real src[DATA_SIZE];

		// transpose matrix
		Transpose(this->data, src);

		// temp array for pairs
		Real tmp[DATA_SIZE];

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
		Real dst[DATA_SIZE];

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
		Real det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

		return det;
	}

	/*
	 * Multiply this matrix by the scalar [scalar]
	 */
	void Matrix4x4::MultiplyByScalar(Real scalar)
	{
		for (UInt32 i = 0; i < DATA_SIZE; i++)
		{
			data[i] *= scalar;
		}
	}

	/*
	 * Transform [vector] by this matrix, and store the result in [out]
	 */
	void Matrix4x4::Transform(const Vector3& vector, Vector3& out) const
	{
		Real * vectorData = const_cast<Vector3&>(vector).GetDataPtr();
		MultiplyMV(this->data, vectorData, out.GetDataPtr());
	}

	/*
	 * Transform [point] by this matrix, and store the result in [out]
	 */
	void Matrix4x4::Transform(const Point3& point, Point3& out) const
	{
		Real * pointData = const_cast<Point3&>(point).GetDataPtr();
		MultiplyMV(this->data, pointData, out.GetDataPtr());
	}

	/*
	 * Transform [vector] by this matrix
	 */
	void Matrix4x4::Transform(Vector3& vector) const
	{
		Real temp[DIM_SIZE];
		MultiplyMV(this->data, vector.GetDataPtr(), temp);
		memcpy(vector.GetDataPtr(), temp, sizeof(Real) * DIM_SIZE);
	}

	/*
	 * Transform [point] by this matrix
	 */
	void Matrix4x4::Transform(Point3& point) const
	{
		Real temp[DIM_SIZE];
		MultiplyMV(this->data, point.GetDataPtr(), temp);
		memcpy(point.GetDataPtr(), temp, sizeof(Real) * DIM_SIZE);
	}

	/*
	 * Transform [vector4f] by this matrix
	 */
	void Matrix4x4::Transform(Real * vector4f) const
	{
		NONFATAL_ASSERT(vector4f != NULL, "Matrix4x4::Transform(Real *) -> 'vector4f' is null.", true);

		Real temp[DIM_SIZE];
		MultiplyMV(this->data, vector4f, temp);
		memcpy(vector4f, temp, sizeof(Real) * DIM_SIZE);
	}

	/*
	 * Add [matrix] to this matrix
	 */
	void Matrix4x4::Add(const Matrix4x4& matrix)
	{
		for (UInt32 i = 0; i < DATA_SIZE; i++)
		{
			data[i] += matrix.data[i];
		}
	}

	/*
	 * Post-multiply this matrix by [matrix]
	 */
	void Matrix4x4::Multiply(const Matrix4x4& matrix)
	{
		Real temp[DATA_SIZE];
		MultiplyMM(this->data, matrix.data, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Pre-multiply this matrix by [matrix]
	 */
	void Matrix4x4::PreMultiply(const Matrix4x4& matrix)
	{
		Real temp[DATA_SIZE];
		MultiplyMM(matrix.data, this->data, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Post-multiply this matrix by [matrix], and store the result in [out]
	 */
	void Matrix4x4::Multiply(const Matrix4x4& matrix, Matrix4x4& out) const
	{
		MultiplyMM(this->data, matrix.data, out.data);
	}

	/*
	 * Post-multiply [lhs] by [rhs], and store the result in [out]
	 */
	void Matrix4x4::Multiply(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& out)
	{
		MultiplyMM(lhs.data, rhs.data, out.data);
	}

	/*
	 * Transform the vector pointed to by [rhsVec], by the matrix pointed to by [lhsMat],
	 * and store the result in [out]
	 */
	void Matrix4x4::MultiplyMV(const Real * lhsMat, const Real * rhsVec, Real * out)
	{
		NONFATAL_ASSERT(lhsMat != NULL, "Matrix4x4::MultiplyMV -> 'lhsMat' is null.", true);
		NONFATAL_ASSERT(rhsVec != NULL, "Matrix4x4::MultiplyMV -> 'rhsVec' is null.", true);
		NONFATAL_ASSERT(out != NULL, "Matrix4x4::MultiplyMV -> 'out' is null.", true);

		Mx4transform(rhsVec[0], rhsVec[1], rhsVec[2], rhsVec[3], lhsMat, out);
	}

	/*
	 * Transform the homogeneous point/vector specified by [x,y,z,w], by the matrix pointed to by [matrix],
	 * and store the result in [pDest]
	 */
	void Matrix4x4::Mx4transform(Real x, Real y, Real z, Real w, const Real* matrix, Real* pDest)
	{
		NONFATAL_ASSERT(matrix != NULL, "Matrix4x4::Mx4transform -> 'lhsMat' is null.", true);
		NONFATAL_ASSERT(pDest != NULL, "Matrix4x4::Mx4transform -> 'pDest' is null.", true);

		pDest[0] = matrix[0 + DIM_SIZE * 0] * x + matrix[0 + DIM_SIZE * 1] * y + matrix[0 + DIM_SIZE * 2] * z + matrix[0 + DIM_SIZE * 3] * w;
		pDest[1] = matrix[1 + DIM_SIZE * 0] * x + matrix[1 + DIM_SIZE * 1] * y + matrix[1 + DIM_SIZE * 2] * z + matrix[1 + DIM_SIZE * 3] * w;
		pDest[2] = matrix[2 + DIM_SIZE * 0] * x + matrix[2 + DIM_SIZE * 1] * y + matrix[2 + DIM_SIZE * 2] * z + matrix[2 + DIM_SIZE * 3] * w;
		pDest[3] = matrix[3 + DIM_SIZE * 0] * x + matrix[3 + DIM_SIZE * 1] * y + matrix[3 + DIM_SIZE * 2] * z + matrix[3 + DIM_SIZE * 3] * w;
	}

	/*********************************************************
	*
	* Multiply two 4x4 matrices (in Real array form) together and store the result in a third 4x4 matrix.
	* In matrix notation: out = lhs x rhs. Due to the way matrix multiplication works,
	* the [out] matrix will have the same effect as first multiplying by the [rhs] matrix,
	* then multiplying by the [lhs] matrix.
	*
	* Parameters:
	* [out] The Real array that holds the result.
	* [lhs] The Real array that holds the left-hand-side 4x4 matrix.
	* [rhs] The Real array that holds the right-hand-side 4x4 matrix.
	*
	*********************************************************/

	void Matrix4x4::MultiplyMM(const Real * lhs, const Real *rhs, Real * out)
	{
		for (Int32 i = 0; i < DIM_SIZE; i++)
		{
			const Real rhs_i0 = rhs[I(i, 0)];
			Real ri0 = lhs[I(0, 0)] * rhs_i0;
			Real ri1 = lhs[I(0, 1)] * rhs_i0;
			Real ri2 = lhs[I(0, 2)] * rhs_i0;
			Real ri3 = lhs[I(0, 3)] * rhs_i0;
			for (Int32 j = 1; j < DIM_SIZE; j++)
			{
				const Real rhs_ij = rhs[I(i, j)];
				ri0 += lhs[I(j, 0)] * rhs_ij;
				ri1 += lhs[I(j, 1)] * rhs_ij;
				ri2 += lhs[I(j, 2)] * rhs_ij;
				ri3 += lhs[I(j, 3)] * rhs_ij;
			}
			out[I(i, 0)] = ri0;
			out[I(i, 1)] = ri1;
			out[I(i, 2)] = ri2;
			out[I(i, 3)] = ri3;
		}
	}

	/*
	 * Transpose this matrix
	 */
	void Matrix4x4::Transpose()
	{
		Real temp[DATA_SIZE];
		Transpose(data, temp);
		memcpy(temp, data, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Transpose the 4x4 matrix pointed to by [source] and store in [dest].
	 */
	void Matrix4x4::Transpose(const Real* source, Real *dest)
	{
		NONFATAL_ASSERT(source != NULL, "Matrix4x4::Transpose -> 'source' is null.", true);
		NONFATAL_ASSERT(dest != NULL, "Matrix4x4::Transpose -> 'dest' is null.", true);

		for (Int32 i = 0; i < DIM_SIZE; i++)
		{
			Int32 mBase = i * DIM_SIZE;
			dest[i] = source[mBase];
			dest[i + DIM_SIZE] = source[mBase + 1];
			dest[i + DIM_SIZE * 2] = source[mBase + 2];
			dest[i + DIM_SIZE * 3] = source[mBase + 3];
		}
	}

	/*
	 * Invert this matrix.
	 *
	 * Returns false if the matrix cannot be inverted
	 */
	bool Matrix4x4::Invert()
	{
		Real temp[DATA_SIZE];
		bool success = Invert(data, temp);
		if (success == true)
		{
			memcpy(data, temp, sizeof(Real) * DATA_SIZE);
		}
		return success;
	}

	/*
	 * Invert the 4x4 matrix pointed to by [out].
	 *
	 * Returns false if the matrix cannot be inverted
	 */
	bool Matrix4x4::Invert(Matrix4x4& out)
	{
		return Invert(data, out.data);
	}

	/*
	 * Invert the 4x4 matrix pointed to by [source] and store the result in [dest]
	 *
	 * Returns false if the matrix cannot be inverted
	 */
	bool Matrix4x4::Invert(const Real * source, Real * dest)
	{
		// we need to know if the matrix is affine so that we can make it affine
		// once again after the inversion. the inversion process can introduce very small
		// precision errors that accumulate over time and eventually
		// result in a non-affine matrix
		bool isAffine = Matrix4x4::IsAffine(source);

		NONFATAL_ASSERT_RTRN(source != NULL, "Matrix4x4::Invert -> 'source' is null.", false, true);
		NONFATAL_ASSERT_RTRN(dest != NULL, "Matrix4x4::Invert -> 'dest' is null.", false, true);

		// array of transpose source matrix
		Real src[DATA_SIZE];

		// transpose matrix
		Transpose(source, src);

		// temp array for pairs
		Real tmp[DATA_SIZE];

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
		Real dst[DATA_SIZE];

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
		Real det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

		if (det == 0.0f)
		{
			return false;
		}

		// calculate matrix inverse
		det = 1 / det;
		for (Int32 j = 0; j < DATA_SIZE; j++)
			dest[j] = dst[j] * det;

		// if the matrix was affine before inversion, make it affine again
		// to avoid accumulating preicision errors
		if (isAffine)
		{
			dest[3] = 0;
			dest[7] = 0;
			dest[11] = 0;
			dest[15] = 1;
		}

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
	void Matrix4x4::SetIdentity(Real * target)
	{
		NONFATAL_ASSERT(target != NULL, "Matrix4x4::SetIdentity -> 'target' is null.", true);

		for (Int32 i = 0; i < DATA_SIZE; i++)
		{
			target[i] = 0;
		}

		for (Int32 i = 0; i < DATA_SIZE; i += 5)
		{
			target[i] = 1.0f;
		}
	}

	/*
	 * Get a pointer the Real data that makes up this matrix
	 */
	const Real * Matrix4x4::GetDataPtr() const
	{
		return (const Real *)data;
	}

	/*
	 * Scale this matrix by the x,y, and z components of [scale]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying this
	 * matrix by a scale matrix
	 */
	void Matrix4x4::Scale(const Vector3& scale)
	{
		Scale(scale.x, scale.y, scale.z);
	}

	/*
	 * Scale this matrix by [x], [y], and [z].
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying this
	 * matrix by a scale matrix
	 */
	void Matrix4x4::Scale(Real x, Real y, Real z)
	{
		Real temp[DATA_SIZE];
		Scale(this->data, temp, x, y, z);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Scale this matrix by [x], [y], and [z].
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying this
	 * matrix by a scale matrix
	 */
	void Matrix4x4::PreScale(Real x, Real y, Real z)
	{
		Real temp[DATA_SIZE];
		Matrix4x4 scale;
		scale.SetIdentity();
		scale.Scale(x, y, z);

		MultiplyMM(scale.GetDataPtr(), this->data, temp);

		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Scale this matrix by [x], [y], and [z] and store the result in [out]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying this
	 * matrix by a scale matrix
	 */
	void Matrix4x4::Scale(Matrix4x4& out, Real x, Real y, Real z) const
	{
		Scale(this->data, out.data, x, y, z);
	}

	/*
	 * Scale the matrix pointed to by [source] by [x], [y], and [z], and store
	 * the result in [dest]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * [source] by a scale matrix
	 */
	void Matrix4x4::Scale(const Real * source, Real * dest, Real x, Real y, Real z)
	{
		NONFATAL_ASSERT(source != NULL, "Matrix4x4::Scale -> 'source' is null.", true);
		NONFATAL_ASSERT(dest != NULL, "Matrix4x4::Scale -> 'dest' is null.", true);

		for (Int32 i = 0; i < DIM_SIZE; i++)
		{
			Int32 smi = i;
			Int32 mi = i;
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
	void Matrix4x4::Translate(const Vector3& vector)
	{
		Real x = vector.x;
		Real y = vector.y;
		Real z = vector.z;
		Translate(x, y, z);
	}

	/*
	 * Translate this matrix by [x], [y], [z]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * this matrix by a translation matrix
	 */
	void Matrix4x4::Translate(Real x, Real y, Real z)
	{
		Translate(data, data, x, y, z);
	}

	/*
	 * Translate this matrix by  [vector].
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
	 * this matrix by a translation matrix
	 */
	void Matrix4x4::PreTranslate(const Vector3& vector)
	{
		PreTranslate(vector.x, vector.y, vector.z);
	}
	/*
	 * Translate this matrix by [x], [y], [z]
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
	 * this matrix by a translation matrix
	 */
	void Matrix4x4::PreTranslate(Real x, Real y, Real z)
	{
		Real dest[DATA_SIZE];
		Matrix4x4::PreTranslate(data, dest, x, y, z);
		memcpy(data, dest, sizeof(Real)*DATA_SIZE);
	}

	/*
	 * Translate the 4x4 matrix pointed to by [source] by [vector], and store in [out]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * [source] by a translation matrix
	 */
	void Matrix4x4::Translate(const Matrix4x4& source, Matrix4x4& out, const Vector3& vector)
	{
		Translate(source.data, out.data, vector.x, vector.y, vector.z);
	}

	/*
	 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [out]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * [source] by a translation matrix
	 */
	void Matrix4x4::Translate(const Matrix4x4& source, Matrix4x4& out, Real x, Real y, Real z)
	{
		Translate(source.data, out.data, x, y, z);
	}

	/*
	 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [dest]
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * [source] by a translation matrix
	 */
	void Matrix4x4::Translate(const Real * source, Real * dest, Real x, Real y, Real z)
	{
		NONFATAL_ASSERT(source != NULL, "Matrix4x4::Translate -> 'source' is null.", true);

		if (source != dest)
		{
			for (Int32 i = 0; i < 12; i++)
			{
				dest[i] = source[i];
			}
		}

		for (Int32 i = 0; i < DIM_SIZE; i++)
		{
			Int32 tmi = i;
			Int32 mi = i;
			dest[12 + tmi] = source[mi] * x + source[4 + mi] * y + source[8 + mi] * z + source[12 + mi];
		}
	}

	/*
	 * Translate the 4x4 matrix pointed to by [source] by [x], [y], and [z], and store in [dest]
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
	 * [source] by a translation matrix
	 */
	void Matrix4x4::PreTranslate(const Real * source, Real * dest, Real x, Real y, Real z)
	{
		Matrix4x4 trans;
		trans.SetIdentity();

		Real *matrixData = const_cast<Real *>(trans.GetDataPtr());
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
	void Matrix4x4::Rotate(const Vector3& vector, Real a)
	{
		Real temp[DATA_SIZE];
		Real r[DATA_SIZE];
		SetRotate(r, vector.x, vector.y, vector.z, a);
		MultiplyMM(data, r, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Rotate this matrix around the [x], [y], [z] axis by [a] degrees
	 *
	 * This performs a post-transformation, in that it is equivalent to post-multiplying
	 * this matrix by a rotation matrix
	 */
	void Matrix4x4::Rotate(Real x, Real y, Real z, Real a)
	{
		Real temp[DATA_SIZE];
		Real r[DATA_SIZE];
		SetRotate(r, x, y, z, a);
		MultiplyMM(data, r, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Rotate this matrix around the [vector] axis by [a] degrees
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
	 * this matrix by a rotation matrix
	 */
	void Matrix4x4::PreRotate(const Vector3& vector, Real a)
	{
		Real temp[DATA_SIZE];
		Real r[DATA_SIZE];
		SetRotate(r, vector.x, vector.y, vector.z, a);
		MultiplyMM(r, data, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Rotate this matrix around the [x], [y], [z] axis by [a] degrees
	 *
	 * This performs a pre-transformation, in that it is equivalent to pre-multiplying
	 * this matrix by a rotation matrix
	 */

	void Matrix4x4::PreRotate(Real x, Real y, Real z, Real a)
	{
		Real temp[DATA_SIZE];
		Real r[DATA_SIZE];
		SetRotate(r, x, y, z, a);
		MultiplyMM(r, data, temp);
		memcpy(data, temp, sizeof(Real) * DATA_SIZE);
	}

	/*
	 * Set this matrix to be a rotation matrix, with Euler angles [x], [y], [z]
	 */
	void Matrix4x4::SetRotateEuler(Real x, Real y, Real z)
	{
		SetRotateEuler(data, x, y, z);
	}

	/*
	 * Set the 4x4 matrix [m] to be a rotation matrix, around axis [x], [y], [z] by [a] degrees
	 */
	void Matrix4x4::SetRotate(Matrix4x4& m, Real x, Real y, Real z, Real a)
	{
		SetRotate(m.data, x, y, z, a);
	}

	/*
	 * Set the 4x4 matrix [rm] to be a rotation matrix, around axis [x], [y], [z] by [a] degrees
	 */
	void Matrix4x4::SetRotate(Real * rm, Real x, Real y, Real z, Real a)
	{
		NONFATAL_ASSERT(rm != NULL, "Matrix4x4::SetRotate -> 'rm' is null.", true);

		rm[3] = 0;
		rm[7] = 0;
		rm[11] = 0;
		rm[12] = 0;
		rm[13] = 0;
		rm[14] = 0;
		rm[15] = 1;
		a *= Constants::DegreesToRads;
		Real s = (Real)GTEMath::Sin(a);
		Real c = (Real)GTEMath::Cos(a);
		if (1.0f == x && 0.0f == y && 0.0f == z)
		{
			rm[5] = c;   rm[10] = c;
			rm[6] = s;   rm[9] = -s;
			rm[1] = 0;   rm[2] = 0;
			rm[4] = 0;   rm[8] = 0;
			rm[0] = 1;
		}
		else if (0.0f == x && 1.0f == y && 0.0f == z)
		{
			rm[0] = c;   rm[10] = c;
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
			rm[10] = 1;
		}
		else
		{
			Real len = Vector3::Magnitude(x, y, z);
			if (1.0f != len)
			{
				Real recipLen = 1.0f / len;
				x *= recipLen;
				y *= recipLen;
				z *= recipLen;
			}
			Real nc = 1.0f - c;
			Real xy = x * y;
			Real yz = y * z;
			Real zx = z * x;
			Real xs = x * s;
			Real ys = y * s;
			Real zs = z * s;
			rm[0] = x*x*nc + c;
			rm[4] = xy*nc - zs;
			rm[8] = zx*nc + ys;
			rm[1] = xy*nc + zs;
			rm[5] = y*y*nc + c;
			rm[9] = yz*nc - xs;
			rm[2] = zx*nc - ys;
			rm[6] = yz*nc + xs;
			rm[10] = z*z*nc + c;
		}
	}

	/*
	 * Set the matrix [rm] to be a rotation matrix, with Euler angles [x], [y], [z]
	 */
	void Matrix4x4::SetRotateEuler(Real * rm, Real x, Real y, Real z)
	{
		NONFATAL_ASSERT(rm != NULL, "Matrix4x4::SetRotateEuler -> 'rm' is null.", true);

		x *= Constants::DegreesToRads;
		y *= Constants::DegreesToRads;
		z *= Constants::DegreesToRads;
		Real cx = (Real)GTEMath::Cos(x);
		Real sx = (Real)GTEMath::Sin(x);
		Real cy = (Real)GTEMath::Cos(y);
		Real sy = (Real)GTEMath::Sin(y);
		Real cz = (Real)GTEMath::Cos(z);
		Real sz = (Real)GTEMath::Sin(z);
		Real cxsy = cx * sy;
		Real sxsy = sx * sy;

		rm[0] = cy * cz;
		rm[1] = -cy * sz;
		rm[2] = sy;
		rm[3] = 0.0f;

		rm[4] = cxsy * cz + cx * sz;
		rm[5] = -cxsy * sz + cx * cz;
		rm[6] = -sx * cy;
		rm[7] = 0.0f;

		rm[8] = -sxsy * cz + sx * sz;
		rm[9] = sxsy * sz + sx * cz;
		rm[10] = cx * cy;
		rm[11] = 0.0f;

		rm[12] = 0.0f;
		rm[13] = 0.0f;
		rm[14] = 0.0f;
		rm[15] = 1.0f;
	}

}