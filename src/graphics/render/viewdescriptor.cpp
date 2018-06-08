#include "viewdescriptor.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE {
    ViewDescriptor::ViewDescriptor() {
        CullingMask = 0;
        ClearBufferMask = 0;

        ReverseCulling = false;

        LightingEnabled = true;
        DepthPassEnabled = true;

        SSAOEnabled = true;
        SSAOMode = SSAORenderMode::Standard;

        SkyboxEnabled = false;
        SkyboxObject = nullptr;

        ClipPlaneCount = 0;
        ClipPlane0Offset = 0.0f;
    }

    ViewDescriptor::~ViewDescriptor() {

    }
}

