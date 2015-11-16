#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include <chrono>
#include "time.h"

namespace GTE
{
	Real Time::timeScale = 1.0f;
	Bool Time::initialized = false;
	unsigned long long Time::startupTime = 0;
	Real Time::lastRecordedTime = 0;
	Real Time::deltaTime = 0;
	std::chrono::high_resolution_clock::time_point Time::_startupTime;

	Time::Time()
	{

	}

	Time::~Time()
	{

	}

	void Time::Initialize()
	{
		if (!initialized)
		{
			_startupTime = std::chrono::high_resolution_clock::now();
			startupTime = _startupTime.time_since_epoch().count();
			initialized = true;
		}
	}

	Real Time::GetRealTimeSinceStartup()
	{
		Initialize();
		std::chrono::high_resolution_clock::time_point _currentTime = std::chrono::high_resolution_clock::now();

		auto elapsed = _currentTime - _startupTime;

		UInt64 d = (UInt64)std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		Real f = (Real)((RealDouble)d / (RealDouble)1000000.0);
		return f;
	}

	Real Time::GetTime()
	{
		return GetRealTimeSinceStartup() * timeScale;
	}

	void Time::Update()
	{
		Initialize();
		deltaTime = GetRealTimeSinceStartup() - lastRecordedTime;
		lastRecordedTime = GetRealTimeSinceStartup();
	}

	Real Time::GetDeltaTime()
	{
		Initialize();
		return deltaTime;
	}

}