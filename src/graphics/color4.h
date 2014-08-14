#ifndef _COLOR4_H_
#define _COLOR4_H_

#include "base/basevector4.h"

class Color4 : public BaseVector4
{
	protected:

	void UpdateComponentPointers();

    public:

    float &r;
    float &g;
    float &b;
    float &a;

    Color4();
    Color4(bool permAttached, float * target);
    Color4(float r, float g, float b, float a);
    Color4(const Color4 * point);
    Color4(const float * data);
    ~Color4();

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
