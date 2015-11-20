
/*
* class: RenderState
*
* author: Mark Kellogg
*
* Several types that are used by the core graphics components.
*/

#ifndef _GTE_RENDERSTATE_H_
#define _GTE_RENDERSTATE_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	class RenderState
	{
		public:

		enum class BlendingMode
		{
			None,
			Additive,
			Custom,
		};

		enum class BlendingMethod
		{
			One,
			SrcAlpha,
			OneMinusSrcAlpha,
			DstAlpha,
			OneMinusDstAlpha,
			Zero
		};

		enum class FaceCulling
		{
			Back = 0,
			Front = 1,
			None = 2
		};

		enum class DepthBufferFunction
		{
			Always = 0,
			Less = 1,
			LessThanOrEqual = 2,
			Greater = 3,
			GreaterThanOrEqual = 4,
			Equal = 5
		};

	};
}

#endif