
/*
* Graphics properties and definitions.
*
* author: Mark Kellogg
*
* Several types that are used by the core graphics components.
*/

#ifndef _GTE_GRAPHICSATTR_H_
#define _GTE_GRAPHICSATTR_H_

#include "engine.h"
#include "global/global.h"

namespace GTE {
    enum class ColorChannel {
        Red = 0,
        Green = 1,
        Blue = 2,
        Alpha = 3
    };

    enum class RenderMode {
        Standard = 1,
        StandardWithShadowVolumeTest = 2,
        ShadowVolumeRender = 3,
        DepthOnly = 4,
        None = 0
    };

    enum class SSAORenderMode {
        Standard = 0,
        Outline = 1
    };

    enum class AntialiasingMethod {
        None = 0,
        MSAAx2 = 1,
        MSAAx4 = 2,
        MSAAx8 = 3,
        MSAAx16 = 4
    };

    /*
     * An instance of GraphicsAttributes is used to define the fundamental
     * attributes of the graphics system.
     */
    class GraphicsAttributes {
    public:

        UInt32 WindowWidth;
        UInt32 WindowHeight;
        mutable UInt32 FramebufferWidth;
        mutable UInt32 FramebufferHeight;
        std::string WindowTitle;
        Bool WaitForVSync;
        AntialiasingMethod AAMethod;

        GraphicsAttributes() {
            WindowWidth = 640;
            WindowHeight = 480;
            FramebufferWidth = WindowWidth;
            FramebufferHeight = WindowHeight;
            WindowTitle = std::string("GTE window");
            WaitForVSync = false;
            AAMethod = AntialiasingMethod::MSAAx2;
        }

        static UInt32 GetMSAASamples(AntialiasingMethod method) {
            switch (method) {
                case AntialiasingMethod::MSAAx2:
                return 2;
                break;
                case AntialiasingMethod::MSAAx4:
                return 4;
                break;
                case AntialiasingMethod::MSAAx8:
                return 8;
                break;
                case AntialiasingMethod::MSAAx16:
                return 16;
                break;
                default:
                return 0;
                break;
            }
        }

        static Bool IsMSAA(AntialiasingMethod method) {
            switch (method) {
                case AntialiasingMethod::MSAAx2:
                case AntialiasingMethod::MSAAx4:
                case AntialiasingMethod::MSAAx8:
                case AntialiasingMethod::MSAAx16:
                return true;
                break;
                default:
                return false;
                break;
            }
        }
    };
}

#endif
