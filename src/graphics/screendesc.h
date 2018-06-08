#ifndef _GTE_SCREENDESC_H_
#define _GTE_SCREENDESC_H_

#include "engine.h"

namespace GTE {
    class ScreenDescriptor {
        Int32 screenWidth;
        Int32 screenHeight;

    public:

        ScreenDescriptor();
        ScreenDescriptor(Int32 screenWidth, Int32 screenHeight);
        ~ScreenDescriptor();

        Int32 GetScreenWidth();
        Int32 getScreenHeight();
    };
}

#endif
