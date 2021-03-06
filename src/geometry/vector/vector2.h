#ifndef _GTE_VECTOR2_H_
#define _GTE_VECTOR2_H_

#include "base/basevector.h"
#include "base/basevectortraits.h"
#include "base/basevectorarray.h"

namespace GTE {
    // forward declarations
    class Vector2;

    template <> class BaseVectorTraits<Vector2> {
    public:
        static const UInt32 VectorSize = 2;
    };

    class Vector2 : public BaseVector<Vector2> {
    protected:

    public:

        Real & x;
        Real &y;

        Vector2();
        Vector2(Bool permAttached, Real * target);
        Vector2(Real u, Real v);
        Vector2(const Vector2& uv);
        ~Vector2();

        Vector2 & operator= (const Vector2 & source);
        BaseVector<Vector2>& operator= (const BaseVector<Vector2>& source);
        void Set(Real x, Real y);
        Real MaxComponentMagnitude();

        void Add(const Vector2& v);
        static void Add(const Vector2& v1, const Vector2& v2, Vector2& result);
        void Multiply(const Vector2& v);
        static void Multiply(const Vector2& a, const Vector2& b, Vector2& results);
        void Lerp(const Vector2& v1, const Vector2& v2, Real t);
        static void Lerp(const Vector2& v1, const Vector2& v2, Vector2& result, Real t);

        void Scale(Real magnitude);
        void Normalize();
        Real Magnitude() const;
        static Real Magnitude(Real x, Real y);

        virtual void AttachTo(Real * data);
        virtual void Detach();
    };

    typedef BaseVectorArray<Vector2> Vector2Array;
}

#endif
