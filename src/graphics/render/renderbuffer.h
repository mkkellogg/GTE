#ifndef _GTE_RENDER_BUFFER_H_
#define _GTE_RENDER_BUFFER_H_

#include "engine.h"
#include "base/bitmask.h"

namespace GTE {
    enum class RenderBufferType {
        Color = 1,
        Depth = 2,
        Stencil = 4
    };
}

#endif

