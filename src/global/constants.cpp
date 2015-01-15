#include "constants.h"

const float Constants::PI = 3.14159265359;
const float Constants::TwoPI = PI * 2;
const float Constants::PIOver360 = PI / 360.0;
const float Constants::TwoPIOver360 = TwoPI / 360.0;
const float Constants::RadsToDegrees = 360.0/TwoPI;
const float Constants::DegreesToRads = TwoPI/360.0;
const std::string Constants::BuiltinShaderPath = std::string("resources/shaders/builtin");
const std::string Constants::BuiltinShaderPathOpenGL = std::string("resources/shaders/builtin/glsl");
