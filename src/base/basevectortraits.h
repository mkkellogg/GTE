#ifndef _GTE_BASEVECTORTRAITS_H_
#define _GTE_BASEVECTORTRAITS_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	class Point3;
	class Vector3;
	class Color4;

	class Vector2;
	class UV2;

	template <class T> class BaseVectorTraits
	{
		public:

		static const UInt32 VectorSize = T::VectorSize;
	};

	template <> class BaseVectorTraits<Point3>
	{
		public:
		static const UInt32 VectorSize = 4;
	};

	template <> class BaseVectorTraits<Vector3>
	{
		public:
		static const UInt32 VectorSize = 4;
	};

	template <> class BaseVectorTraits<Color4>
	{
		public:
		static const UInt32 VectorSize = 4;
	};

	template <> class BaseVectorTraits<UV2>
	{
		public:
		static const UInt32 VectorSize = 2;
	};

	template <> class BaseVectorTraits<Vector2>
	{
		public:
		static const UInt32 VectorSize = 2;
	};
}

#endif
