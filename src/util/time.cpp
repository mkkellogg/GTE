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
		startupTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::microseconds(1);
		initialized = true;
	}
}

float Time::GetRealTimeSinceStartup()
{
	Initialize();
	unsigned long long currentTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::microseconds(1);
	unsigned long long longDiff = currentTime - startupTime;

	return (float)((float)longDiff/(float)1000000.0);
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
