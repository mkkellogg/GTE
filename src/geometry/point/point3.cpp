#include "point3.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

namespace GTE {
    /*
     * Default constructor
     */
    Point3::Point3() : BaseVector<Point3>(), x(data[0]), y(data[1]), z(data[2]) {
        data[3] = 1;
    }

    /*
     * Constructor will alternate backing storage
     */
    Point3::Point3(Bool permAttached, Real * target) : BaseVector<Point3>(permAttached, target), x(data[0]), y(data[1]), z(data[2]) {
        data[3] = 1;
    }

    /*
     * Constructor with initialization values
     */
    Point3::Point3(Real x, Real y, Real z) : BaseVector<Point3>(), x(data[0]), y(data[1]), z(data[2]) {
        Set(x, y, z);
    }

    /*
     * Copy constructor
     */
    Point3::Point3(const Point3& point) : BaseVector<Point3>(point), x(data[0]), y(data[1]), z(data[2]) {

    }

    /*
     * Clean up
     */
    Point3::~Point3() {

    }

    /*
    * Assignment operator
    */
    Point3& Point3::operator= (const Point3& source) {
        if (this == &source)return *this;
        BaseVector<Point3>::operator=(source);
        return *this;
    }

    /*
    * Comparison operator
    */
    Bool Point3::operator==(const Point3 & source) {
        Real epsilon = .005f;
        return GTEMath::Abs(source.x - x) < epsilon && GTEMath::Abs(source.y - y) < epsilon && GTEMath::Abs(source.z - z) < epsilon;
    }

    /*
    * Comparison operator
    */
    Bool Point3::operator==(const Point3& p) const {
        Real epsilon = .005f;
        return GTEMath::Abs(p.x - this->x) < epsilon && GTEMath::Abs(p.y - this->y) < epsilon && GTEMath::Abs(p.z - this->z) < epsilon;
    }

    /*
    * Comparison function for references
    */
    Bool Point3::AreEqual(const Point3& a, const Point3& b) {
        return AreEqual(&a, &b);
    }

    /*
    * Comparison function for pointers
    */
    Bool Point3::AreEqual(const Point3* a, const Point3* b) {
        NONFATAL_ASSERT_RTRN(a != nullptr && b != nullptr, "Point3::AreEqual -> Null point passed.", false, true);

        Real epsilon = .0005f;
        return GTEMath::Abs(a->x - b->x) < epsilon && GTEMath::Abs(a->y - b->y) < epsilon && GTEMath::Abs(a->z - b->z) < epsilon;
    }

    /*
    * Test for exact equality
    */
    Bool Point3::AreStrictlyEqual(const Point3* a, const Point3* b) {
        NONFATAL_ASSERT_RTRN(a != nullptr && b != nullptr, "Point3::AreStrictlyEqual -> Null point passed.", false, true);
        return a->x == b->x && a->y == b->y && a->z == b->z;
    }

    /*
    * Set the values of this point
    */
    void Point3::Set(Real x, Real y, Real z) {
        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
        this->data[3] = 1;
    }

    /*
    * Get the maximum of the respective absolute value of each component.
    */
    Real Point3::MaxComponentMagnitude() {
        return GTEMath::Max(GTEMath::Abs(x), GTEMath::Max(GTEMath::Abs(y), GTEMath::Abs(z)));
    }

    /*
     * Move this point by [v]
     */
    void Point3::Add(const Vector3& v) {
        Add(*this, v, *this);
    }

    /*
    * Add point [p] to this point.
    */
    void Point3::Add(const Point3& p) {
        Add(*this, p, *this);
    }

    /*
     * Add vector [v] to [point] and store the result in [result]
     */
    void Point3::Add(const Point3& point, const Vector3& v, Point3& result) {
        result.x = point.x + v.x;
        result.y = point.y + v.y;
        result.z = point.z + v.z;
    }

    /*
    * Add point [p] to [point] and store the result in [result]
    */
    void Point3::Add(const Point3& point, const Point3& p, Point3& result) {
        result.x = point.x + p.x;
        result.y = point.y + p.y;
        result.z = point.z + p.z;
    }

    /*
    * Subtract vector [v] from this point.
    */
    void Point3::Subtract(const Vector3& v) {
        Subtract(*this, v, *this);
    }

    /*
    * Subtract [v1] from [p1] and store the resulting point in [result]
    */
    void Point3::Subtract(const Point3& p1, const Vector3& v1, Point3&  result) {
        result.x = p1.x - v1.x;
        result.y = p1.y - v1.y;
        result.z = p1.z - v1.z;
    }

    /*
     * Subtract [p2] from [p1] and store the resulting vector in [result]
     */
    void Point3::Subtract(const Point3& p1, const Point3& p2, Vector3& result) {
        result.x = p1.x - p2.x;
        result.y = p1.y - p2.y;
        result.z = p1.z - p2.z;
    }

    /*
    * Component-wise multiply this point with [p].
    */
    void Point3::Multiply(const Point3& p) {
        Multiply(*this, p, *this);
    }

    /*
    * Component-wise multiply [a] with [b], and store the result in [result].
    */
    void Point3::Multiply(const Point3& a, const Point3& b, Point3& result) {
        result.x = a.x * b.x;
        result.y = a.y * b.y;
        result.z = a.z * b.z;
    }

    /*
    * Linearly interpolate this point from [p1] to [p2].
    */
    void Point3::Lerp(const Point3& p1, const Point3& p2, Real t) {
        Lerp(p1, p2, *this, t);
    }

    /*
     * Linearly interpolate from [p1] to [p2] and store the result in [result]
     */
    void Point3::Lerp(const Point3& p1, const Point3& p2, Point3& result, Real t) {
        result.x = ((p2.x - p1.x) * t) + p1.x;
        result.y = ((p2.y - p1.y) * t) + p1.y;
        result.z = ((p2.z - p1.z) * t) + p1.z;
    }

    /*
    * Scale this point by [magnitude]
    */
    void Point3::Scale(Real magnitude) {
        x *= magnitude;
        y *= magnitude;
        z *= magnitude;
    }

    /*
    * Convert this point to its equivalent inside a unit circle.
    */
    void Point3::Normalize() {
        Real magnitude = x * x + y * y + z * z;
        if (magnitude != 0) {
            Scale(1 / magnitude);
        }
    }

    /*
    * Apply a projection using [mvpMatrix]. This means treating this point
    * as a 4-component vector, multiplying by [mvpMatrix] and then dividing
    * through by the 4th component.
    */
    void Point3::ApplyProjection(const Matrix4x4& mvpMatrix) {
        mvpMatrix.Transform(*this);
        Real w = GetDataPtr()[3];
        x /= w;
        y /= w;
        z /= w;
    }

    /*
    * Override BaseVector::AttachTo() and force it to DO NOTHING. If we allowed the backing storage
    * to change then the references x, y, & z would point to invalid locations.
    */
    void Point3::AttachTo(Real * data) {

    }

    /*
    * Override BaseVector::Detach() and force it to DO NOTHING. If we allowed the backing storage
    * to change then the references x, y, & z would point to invalid locations.
    */
    void Point3::Detach() {

    }
}
