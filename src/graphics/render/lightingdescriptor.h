/*
* class: LightingDescriptor
*
* author: Mark Kellogg
*
* Data structure that is used by render managers for describing lighting
* information to be used during rendering.
*/

#ifndef _GTE_LIGHTING_DESCRIPTOR_H_
#define _GTE_LIGHTING_DESCRIPTOR_H_

#include "engine.h"
#include "object/engineobject.h"

namespace GTE {
    //forward declarations
    class Point3;
    class Vector3;
    class Color4;

    class LightingDescriptor {
        Bool initialized;

    public:

        UInt32 LightCount;
        Light** LightObjects;

        Real* PositionDatas;
        Real* DirectionDatas;
        Real* ColorDatas;
        Point3 * Positions;
        Vector3 * Directions;
        Color4 * Colors;
        Int32* Types;
        Real* Intensities;
        Real* Ranges;
        Real* Attenuations;
        Int32* ParallelAngleAttenuations;
        Int32* OrthoAngleAttenuations;
        Int32* Enabled;

        Bool UseLighting;

        LightingDescriptor();
        ~LightingDescriptor();

        Bool Init(UInt32 maxLights);
    };
}

#endif
