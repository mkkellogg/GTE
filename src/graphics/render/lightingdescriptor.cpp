#include "lightingdescriptor.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE {
    LightingDescriptor::LightingDescriptor() {
        LightObjects = nullptr;

        Positions = nullptr;
        Directions = nullptr;
        Colors = nullptr;

        PositionDatas = nullptr;
        DirectionDatas = nullptr;
        ColorDatas = nullptr;

        Types = nullptr;
        Intensities = nullptr;
        Ranges = nullptr;
        Attenuations = nullptr;
        ParallelAngleAttenuations = nullptr;
        OrthoAngleAttenuations = nullptr;

        Enabled = nullptr;

        initialized = false;
        UseLighting = true;
        LightCount = 0;
    }

    LightingDescriptor::~LightingDescriptor() {
        SAFE_DELETE(LightObjects);

        if (Positions != nullptr) {
            delete[] Positions;
            Positions = nullptr;
        }

        if (Directions != nullptr) {
            delete[] Directions;
            Directions = nullptr;
        }

        if (Colors != nullptr) {
            delete[] Colors;
            Colors = nullptr;
        }

        SAFE_DELETE(PositionDatas);
        SAFE_DELETE(DirectionDatas);
        SAFE_DELETE(ColorDatas);

        SAFE_DELETE(Types);
        SAFE_DELETE(Intensities);
        SAFE_DELETE(Ranges);
        SAFE_DELETE(Attenuations);
        SAFE_DELETE(ParallelAngleAttenuations);
        SAFE_DELETE(OrthoAngleAttenuations);

        SAFE_DELETE(Enabled);
    }

    Bool LightingDescriptor::Init(UInt32 maxLights) {
        if (!initialized) {
            LightObjects = new Light*[maxLights];
            ASSERT(LightObjects != nullptr, "LightingDescriptor::Init -> Unable to allocate light objects.");

            Positions = new Point3[maxLights];
            ASSERT(Positions != nullptr, "LightingDescriptor::Init -> Unable to allocate light positions.");

            Directions = new Vector3[maxLights];
            ASSERT(Directions != nullptr, "LightingDescriptor::Init -> Unable to allocate light directions.");

            Colors = new Color4[maxLights];
            ASSERT(Colors != nullptr, "LightingDescriptor::Init -> Unable to allocate light colors.");

            PositionDatas = new Real[maxLights * 4];
            ASSERT(PositionDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light position data.");

            DirectionDatas = new Real[maxLights * 4];
            ASSERT(DirectionDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light direction data.");

            ColorDatas = new Real[maxLights * 4];
            ASSERT(ColorDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light color data.");

            Types = new Int32[maxLights];
            ASSERT(Types != nullptr, "LightingDescriptor::Init -> Unable to allocate light types.");

            Intensities = new Real[maxLights];
            ASSERT(Intensities != nullptr, "LightingDescriptor::Init -> Unable to allocate light intensities.");

            Ranges = new Real[maxLights];
            ASSERT(Ranges != nullptr, "LightingDescriptor::Init -> Unable to allocate light ranges.");

            Attenuations = new Real[maxLights];
            ASSERT(Attenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light attenuations.");

            OrthoAngleAttenuations = new Int32[maxLights];
            ASSERT(OrthoAngleAttenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light ortho-angle attenuations.");

            ParallelAngleAttenuations = new Int32[maxLights];
            ASSERT(ParallelAngleAttenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light parallel angle attenuations.");

            Enabled = new Int32[maxLights];
            ASSERT(Enabled != nullptr, "LightingDescriptor::Init -> Unable to allocate 'Enabled' array.");

            initialized = true;
        }

        return true;
    }
}

