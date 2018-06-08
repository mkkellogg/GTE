#include "color4.h"
#include "gtemath/gtemath.h"

namespace GTE {
    Color4::Color4() : BaseVector<Color4>(), r(data[0]), g(data[1]), b(data[2]), a(data[3]) {

    }

    Color4::Color4(Bool permAttached, Real * target) : BaseVector<Color4>(permAttached, target), r(data[0]), g(data[1]), b(data[2]), a(data[3]) {


    }

    Color4::Color4(Real r, Real g, Real b, Real a) : BaseVector<Color4>(), r(data[0]), g(data[1]), b(data[2]), a(data[3]) {
        Set(r, g, b, a);
    }

    Color4::Color4(const Color4& color) : BaseVector<Color4>(color), r(data[0]), g(data[1]), b(data[2]), a(data[3]) {

    }

    Color4::~Color4() {

    }

    /*
     * Assignment operator
     */
    Color4 & Color4::operator= (const Color4& source) {
        if (this == &source)return *this;
        BaseVector<Color4>::operator=(source);
        return *this;
    }

    /*
     * Over-ridden assignment operator from BaseVector4
     */
    BaseVector<Color4>& Color4::operator= (const BaseVector<Color4>& source) {
        if (this == &source)return *this;
        BaseVector<Color4>::operator=(source);
        return *this;
    }

    /*
    * Set the values for each component of this color.
    */
    void Color4::Set(Real r, Real g, Real b, Real a) {
        data[0] = r;
        data[1] = g;
        data[2] = b;
        data[3] = a;
    }

    /*
    * Get the maximum of the respective absolute value of each component.
    */
    Real Color4::MaxComponentMagnitude() {
        return GTEMath::Max(GTEMath::Abs(r), GTEMath::Max(GTEMath::Abs(g), GTEMath::Max(GTEMath::Abs(b), GTEMath::Abs(a))));
    }

    /*
    * Add color [c] to this color
    */
    void Color4::Add(const Color4& c) {
        Add(*this, c, *this);
    }

    /*
    * Add [c1] to [c2] and store the result in [result]
    */
    void Color4::Add(const Color4& c1, const Color4& c2, Color4& result) {
        result.r = c1.r + c2.r;
        result.g = c1.g + c2.g;
        result.b = c1.b + c2.b;
        result.a = c1.a + c2.a;
    }

    /*
    * Component-wise multiply this color with [c].
    */
    void Color4::Multiply(const Color4& c) {
        Multiply(*this, c, *this);
    }

    /*
    * Component-wise multiply [c1] with [c2], and store the result in [result].
    */
    void Color4::Multiply(const Color4& c1, const Color4& c2, Color4& result) {
        result.r = c1.r * c2.r;
        result.g = c1.g * c2.g;
        result.b = c1.b * c2.b;
        result.a = c1.a * c2.a;
    }

    /*
    * Linearly interpolate this color from [c1] to [c2].
    */
    void Color4::Lerp(const Color4& c1, const Color4& c2, Real t) {
        Lerp(c1, c2, *this, t);
    }

    /*
    * Linearly interpolate from [c1] to [c2] and store the result in [result]
    */
    void Color4::Lerp(const Color4& c1, const Color4& c2, Color4& result, Real t) {
        result.r = ((c2.r - c1.r) * t) + c1.r;
        result.g = ((c2.g - c1.g) * t) + c1.g;
        result.b = ((c2.b - c1.b) * t) + c1.b;
        result.a = ((c2.a - c1.a) * t) + c1.a;
    }

    /*
    * Scale this vector by [magnitude]
    */
    void Color4::Scale(Real magnitude) {
        r *= magnitude;
        g *= magnitude;
        b *= magnitude;
        a *= magnitude;
    }

    /*
    * Normalize this color similar to normalizing a vector.
    */
    void Color4::Normalize() {
        Real magnitude = r * r + g * g + b * b + a * a;
        if (magnitude != 0) {
            Scale(1 / magnitude);
        }
    }

    /*
    * Override BaseVector4::AttachTo() and force it to DO NOTHING. If we allowed the backing storage
    * to change then the references r, g, b, & a would point to invalid locations.
    */
    void Color4::AttachTo(Real * data) {

    }

    /*
    * Override BaseVector4::Detach() and force it to DO NOTHING. If we allowed the backing storage
    * to change then the references r, g, b, & a would point to invalid locations.
    */
    void Color4::Detach() {

    }
}
