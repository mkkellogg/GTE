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

#include "engine.h"

namespace GTE
{
	class Time
	{
		static Real timeScale;

		static Bool initialized;
		static unsigned long long startupTime;
		static std::chrono::high_resolution_clock::time_point _startupTime;
		static Real lastRecordedTime;
		static Real lastRecordedRealTime;
		static Real deltaTime;
		static Real realDeltaTime;

		static void Initialize();

		Time();
		~Time();

	protected:

	public:

		static void Update();

		static Real GetRealTimeSinceStartup();
		static Real GetTime();
		static Real GetRealDeltaTime();
		static Real GetDeltaTime();

	};
}

#endif
