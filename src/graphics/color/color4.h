#ifndef _GTE_COLOR4_H_
#define _GTE_COLOR4_H_

#include "base/basevector4.h"

namespace GTE
{
	class Color4 : public BaseVector4
	{
	protected:

	public:

		float &r;
		float &g;
		float &b;
		float &a;

		Color4();
		Color4(bool permAttached, float * target);
		Color4(float r, float g, float b, float a);
		Color4(const Color4& point);
		Color4(const float * data);
		~Color4();

		Color4 & operator= (const Color4 & source);
		BaseVector4 & operator= (const BaseVector4 & source);

		void Set(float r, float g, float b, float a);

		void AttachTo(float * data);
		void Detach();
	};
}

#endif
