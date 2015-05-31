/*
 * class: Time
 *
 * author: Mark Kellogg
 *
 * This class contains the timing functions used by the engine.
 *
 */

#ifndef _GTE_TIME_H_
#define _GTE_TIME_H_

#include <chrono>
#include "object/enginetypes.h"

namespace GTE
{
	class Time
	{
		static Bool initialized;
		static unsigned long long startupTime;
		static std::chrono::high_resolution_clock::time_point _startupTime;
		static Real lastRecordedTime;
		static Real deltaTime;

		static void Initialize();

		Time();
		~Time();

	protected:

	public:

		static void Update();

		static Real GetRealTimeSinceStartup();
		static Real GetDeltaTime();
	};
}

#endif
