#ifndef _GTE_CONSTANTS_H_
#define _GTE_CONSTANTS_H_

#include <string>

namespace GTE
{
	class Constants
	{
	public:

		static const float PI;
		static const float TwoPI;
		static const float PIOver360;
		static const float TwoPIOver360;
		static const float RadsToDegrees;
		static const float DegreesToRads;
		static const unsigned int MaxObjectRecursionDepth = 512;
		static const unsigned int MaxBonesPerVertex = 4;
		static const std::string BuiltinShaderPath;
		static const std::string BuiltinShaderPathOpenGL;
		static const unsigned int MaxClipPlanes = 1;
	};
}

#endif
