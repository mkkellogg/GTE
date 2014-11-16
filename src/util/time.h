/*
 * class: Time
 *
 * author: Mark Kellogg
 *
 * This class contains the timing functions used by the engine.
 *
 */

#ifndef _TIME_H_
#define _TIME_H_

//forward declarations

#include <string>

class Time
{
	static bool initialized;
	static unsigned long long startupTime;
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
