#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include <chrono>
#include "time.h"

bool Time::initialized = false;
unsigned long long Time::startupTime = 0;
float Time::lastRecordedTime = 0;
float Time::deltaTime = 0;

Time::Time()
{

}

Time::~Time()
{

}

void Time::Initialize()
{
	if(!initialized)
	{
		auto _startupTime = std::chrono::high_resolution_clock::now();
		startupTime = _startupTime.time_since_epoch().count();
		initialized = true;
	}
}

float Time::GetRealTimeSinceStartup()
{
	Initialize();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	unsigned long long currentTime = _currentTime.time_since_epoch().count();
	unsigned long long longDiff = currentTime - startupTime;

	return (float)((float)longDiff / (float)10000000.0);
}

void Time::Update()
{
	Initialize();
	deltaTime = GetRealTimeSinceStartup() - lastRecordedTime;
	lastRecordedTime = GetRealTimeSinceStartup();
}

float Time::GetDeltaTime()
{
	Initialize();
	return deltaTime;
}
