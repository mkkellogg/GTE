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
