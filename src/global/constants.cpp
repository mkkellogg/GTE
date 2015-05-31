#include "constants.h"
#include "global/global.h"

namespace GTE
{
	const Real Constants::PI = 3.14159265358979;
	const Real Constants::TwoPI = PI * 2;
	const Real Constants::PIOver360 = PI / 360.0;
	const Real Constants::TwoPIOver360 = TwoPI / 360.0;
	const Real Constants::RadsToDegrees = 360.0 / TwoPI;
	const Real Constants::DegreesToRads = TwoPI / 360.0;
	const std::string Constants::BuiltinShaderPath = std::string("resources/shaders/builtin");
	const std::string Constants::BuiltinShaderPathOpenGL = std::string("resources/shaders/builtin/glsl");
}
