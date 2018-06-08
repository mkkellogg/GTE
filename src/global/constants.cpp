#include "engine.h"
#include "constants.h"
#include "global/global.h"

namespace GTE {
    const Real Constants::PI = 3.14159265358979f;
    const Real Constants::TwoPI = Constants::PI * 2.0f;
    const Real Constants::PIOver360 = Constants::PI / 360.0f;
    const Real Constants::TwoPIOver360 = Constants::TwoPI / 360.0f;
    const Real Constants::RadsToDegrees = 360.0f / Constants::TwoPI;
    const Real Constants::DegreesToRads = Constants::TwoPI / 360.0f;
    const Real Constants::RealToDoubleRatio = sizeof(GTE::RealDouble) / sizeof(GTE::Real);
    const std::string Constants::BuiltinShaderPath = std::string("resources/shaders/builtin");
    const std::string Constants::BuiltinShaderPathOpenGL = std::string("resources/shaders/builtin/glsl");
}
