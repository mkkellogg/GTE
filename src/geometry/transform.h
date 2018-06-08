/*
 * class: Transform
 *
 * author: Mark Kellogg
 *
 * A Transform object encapsulates a Matrix4x4 object and provides convenience
 * functions and wrappers for it.
 */

#ifndef _GTE_TRANSFORM_H_
#define _GTE_TRANSFORM_H_

#include "engine.h"
#include "matrix4x4.h"
#include "global/global.h"

namespace GTE {
    class Transform {

    protected:

        // the 4x4 matrix that is encapsulated by this transform
        Matrix4x4 matrix;

    public:

        Transform();
        Transform(const Transform & source);
        Transform(const Transform *);
        virtual ~Transform();

        Transform& operator= (const Transform & source);

        void CopyMatrix(Matrix4x4& dest) const;
        void SetTo(const Matrix4x4& matrix);
        void SetTo(const Transform& transform);
        void SetTo(const Real * matrixData);
        void SetIdentity();
        void Invert();
        void TransformBy(const Matrix4x4& matrix);
        void PreTransformBy(const Matrix4x4& matrix);
        void TransformBy(const Transform& transform);
        void PreTransformBy(const Transform& transform);

        Matrix4x4& GetMatrix();
        const Matrix4x4& GetConstMatrix() const;

        virtual void Translate(Real x, Real y, Real z, Bool local);
        virtual void Translate(Vector3& vector, Bool local);
        virtual void RotateAround(const Point3& point, const Vector3& axis, Real angle, Bool local);
        virtual void RotateAround(Real px, Real py, Real pz, Real ax, Real ay, Real az, Real angle, Bool local);
        virtual void Scale(const Vector3& mag, Bool local);
        virtual void Scale(Real x, Real y, Real z, Bool local);
        virtual void Rotate(const Vector3& vector, Real a, Bool local);
        virtual void Rotate(Real x, Real y, Real z, Real a, Bool local);

        virtual void TransformVector(Vector3& vector) const;
        virtual void TransformPoint(Point3& point3) const;
        virtual void TransformVector4f(Real * vector) const;

        static Transform * CreateIdentityTransform();
        static void BuildOrthographicProjectionMatrix(Matrix4x4& matrix, Real top, Real bottom, Real left, Real right, Real near, Real far);
        static void BuildPerspectiveProjectionMatrix(Matrix4x4& matrix, Real fov, Real ratio, Real nearP, Real farP);
        static void BuildPerspectiveProjectionMatrixInfiniteFar(Matrix4x4& matrix, Real fov, Real ratio, Real nearP);
        static void BuildLookAtMatrix(Matrix4x4& matrix, Real posX, Real posY, Real posZ, Real lookAtX, Real lookAtY, Real lookAtZ);
    };
}

#endif
