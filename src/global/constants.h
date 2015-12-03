#ifndef _GTE_CONSTANTS_H_
#define _GTE_CONSTANTS_H_

#include "engine.h"
#include "global/global.h"

#include <string>

namespace GTE
{
	class Constants
	{
	public:

		static const UInt32 MaxSceneObjects = 4096;
		
		static const Real PI;
		static const Real TwoPI;
		static const Real PIOver360;
		static const Real TwoPIOver360;
		static const Real RadsToDegrees;
		static const Real DegreesToRads;
		static const std::string BuiltinShaderPath;
		static const std::string BuiltinShaderPathOpenGL;

		static const UInt32 MaxObjectRecursionDepth = 512;
		static const UInt32 MaxBonesPerVertex = 4;
		static const UInt32 MaxClipPlanes = 1;

		static const UInt32 MaxSceneLights = 16;
		static const UInt32 MaxShaderLights = 8;

		static const Real RealToDoubleRatio;
	};
}

#endif
