#ifndef _GTE_RENDER_BUFFER_H_
#define _GTE_RENDER_BUFFER_H_

#include "base/intmask.h"
#include "object/enginetypes.h"

namespace GTE
{
	enum class RenderBufferType
	{
		Color = 1,
		Depth = 2,
		Stencil = 4
	};
}

#endif

