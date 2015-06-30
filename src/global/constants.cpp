#include "constants.h"
#include "global/global.h"

namespace GTE
{
	const Real Constants::PI = 3.14159265358979f;
	const Real Constants::TwoPI = PI * 2;
	const Real Constants::PIOver360 = PI / 360.0f;
	const Real Constants::TwoPIOver360 = TwoPI / 360.0f;
	const Real Constants::RadsToDegrees = 360.0f / TwoPI;
	const Real Constants::DegreesToRads = TwoPI / 360.0f;
	const std::string Constants::BuiltinShaderPath = std::string("resources/shaders/builtin");
	const std::string Constants::BuiltinShaderPathOpenGL = std::string("resources/shaders/builtin/glsl");
}
