#include "Time.h"

Time::Time()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

bool Time::Start()
{
	if (isRunning)
		return false;
	else
	{
		start = std::chrono::high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}

bool Time::Stop()
{
	if (!isRunning)
		return false;
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

void Time::Restart()
{
	isRunning = true;
	start = std::chrono::high_resolution_clock::now();
}

double Time::GetMillisecondsElapsed()
{
	return 0.0;
}
