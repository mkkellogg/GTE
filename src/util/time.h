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

class Time
{
	static bool initialized;
	static unsigned long long startupTime;
	static std::chrono::high_resolution_clock::time_point _startupTime;
	static float lastRecordedTime;
	static float deltaTime;

	static void Initialize();

	Time();
	~Time();

	protected:

	public:

	static void Update();

	static float GetRealTimeSinceStartup();
	static float GetDeltaTime();
};

#endif
